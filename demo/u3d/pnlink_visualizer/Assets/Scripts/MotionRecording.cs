using System;
using System.IO;
using System.Text;

// Versioned, seekable pose format. Each frame has a timestamp and local xyz/xyzw per bone.
// Disk streaming bounds memory; a .partial file can be recovered after an interrupted recording.
public sealed class MotionRecordingWriter : IDisposable
{
    internal static readonly byte[] Magic = Encoding.ASCII.GetBytes("NVPOSE1\n");
    BinaryWriter writer;
    public readonly string Path;
    readonly int bones;
    readonly bool withStatus;
    double previous = -1;
    public int Frames { get; private set; }
    public long Bytes { get { return writer == null ? 0 : writer.BaseStream.Position; } }
    public MotionRecordingWriter(string path, string[] paths, bool withStatus=false)
    {
        if (paths.Length < 1 || paths.Length > 2048) throw new InvalidDataException("骨骼数量无效。");
        Path = path; bones = paths.Length; this.withStatus=withStatus;
        writer = new BinaryWriter(new FileStream(path,FileMode.CreateNew,FileAccess.Write,FileShare.Read,262144));
        try {
            writer.Write(withStatus?Encoding.ASCII.GetBytes("NVPOSE2\n"):Magic); writer.Write(bones);
            foreach (string name in paths) {
                byte[] bytes = Encoding.UTF8.GetBytes(name);
                if (bytes.Length > 4096) throw new InvalidDataException("骨骼路径过长。");
                writer.Write(bytes.Length); writer.Write(bytes);
            }
            writer.Flush();
        } catch { writer.Dispose(); writer = null; throw; }
    }
    public void Write(double seconds, float[] pose, byte[] status=null)
    {
        if (writer == null || pose.Length != bones*7 || !Finite(seconds) || seconds < 0 || seconds <= previous)
            throw new InvalidDataException("动作帧时间或骨骼数量无效。");
        foreach (float value in pose) if (!Finite(value)) throw new InvalidDataException("动作数据包含非有限数值。");
        if(status!=null && status.Length>262144)throw new InvalidDataException("Device snapshot too large");
        writer.Write(seconds); foreach (float value in pose) writer.Write(value);
        if(withStatus) { writer.Write(status==null?0:status.Length); if(status!=null)writer.Write(status); }
        previous = seconds; ++Frames;
        if (Frames % 120 == 0) writer.Flush();
    }
    internal static bool Finite(double x) { return !double.IsNaN(x) && !double.IsInfinity(x); }
    public void Dispose() { if (writer != null) { writer.Dispose(); writer = null; } }
}

public sealed class MotionRecordingReader : IDisposable
{
    BinaryReader reader;
    long start;
    int stride;
    readonly double[] times;
    readonly long[] offsets;
    readonly System.Collections.Generic.List<int> snapshotFrames=new System.Collections.Generic.List<int>();
    readonly System.Collections.Generic.List<DeviceSnapshot> snapshots=new System.Collections.Generic.List<DeviceSnapshot>();
    public readonly int Version;
    public readonly string[] Paths;
    public readonly bool Recovered;
    public int Frames { get { return times.Length; } }
    public double Duration { get { return times[times.Length-1] - times[0]; } }
    public MotionRecordingReader(string path, System.Threading.CancellationToken cancellation = default(System.Threading.CancellationToken))
    {
        reader = new BinaryReader(new FileStream(path,FileMode.Open,FileAccess.Read,FileShare.Read,65536));
        try {
            if(reader.BaseStream.Length>3L*1024*1024*1024)throw new InvalidDataException("Recording exceeds the supported size");
            byte[] magic = reader.ReadBytes(8);
            string signature=Encoding.ASCII.GetString(magic);
            Version=signature=="NVPOSE1\n"?1:signature=="NVPOSE2\n"?2:0;
            if(Version==0)throw new InvalidDataException("不是 Visualizer 动作录制文件。");
            int count = reader.ReadInt32();
            if (count < 1 || count > 2048) throw new InvalidDataException("骨骼数量无效。");
            Paths = new string[count]; var unique = new System.Collections.Generic.HashSet<string>();
            for (int i=0;i<count;i++) {
                int length = reader.ReadInt32();
                if (length < 0 || length > 4096) throw new InvalidDataException("骨骼路径无效。");
                var bytes = reader.ReadBytes(length); if (bytes.Length != length) throw new EndOfStreamException();
                Paths[i] = new UTF8Encoding(false,true).GetString(bytes);
                if (!unique.Add(Paths[i])) throw new InvalidDataException("骨骼路径重复。");
            }
            start = reader.BaseStream.Position; stride = 8+count*28;
            var positions=new System.Collections.Generic.List<long>();
            var stamps=new System.Collections.Generic.List<double>();
            Recovered=path.EndsWith(".partial",StringComparison.OrdinalIgnoreCase);
            double previous=-1;
            while(reader.BaseStream.Position<reader.BaseStream.Length) {
                if((positions.Count&255)==0)cancellation.ThrowIfCancellationRequested();
                if(positions.Count>=1000000)throw new InvalidDataException("录制过长。");
                long frameStart=reader.BaseStream.Position;
                if(reader.BaseStream.Length-frameStart<stride+(Version==2?4:0)){Recovered=true;break;}
                double t=reader.ReadDouble();
                if(!MotionRecordingWriter.Finite(t)||t<0||t<=previous||t>86400)throw new InvalidDataException("录制时间轴无效。");
                for(int j=0;j<count;j++) {
                    for(int k=0;k<3;k++)if(!MotionRecordingWriter.Finite(reader.ReadSingle()))throw new InvalidDataException("位置数据无效。");
                    double norm=0;
                    for(int k=0;k<4;k++){float v=reader.ReadSingle();if(!MotionRecordingWriter.Finite(v))throw new InvalidDataException("旋转数据无效。");norm+=v*v;}
                    if(norm<.5||norm>1.5)throw new InvalidDataException("旋转四元数无效。");
                }
                DeviceSnapshot snapshot=null;
                if(Version==2) {
                    int length=reader.ReadInt32();
                    if(length<0||length>262144)throw new InvalidDataException("Invalid device snapshot length");
                    if(length>reader.BaseStream.Length-reader.BaseStream.Position){Recovered=true;break;}
                    if(length>0)snapshot=DeviceSnapshot.Decode(reader.ReadBytes(length));
                }
                if(snapshot!=null){if(snapshots.Count>=20000)throw new InvalidDataException("Too many device snapshots");snapshotFrames.Add(positions.Count);snapshots.Add(snapshot);}
                positions.Add(frameStart);stamps.Add(t);previous=t;
            }
            if(positions.Count==0)throw new InvalidDataException("没有完整帧或录制过长。");
            offsets=positions.ToArray();times=stamps.ToArray();
        } catch { Dispose(); throw; }
    }
    public int FrameAt(double seconds)
    {
        double value = Math.Max(0, Math.Min(Duration,seconds)) + times[0];
        int index = Array.BinarySearch(times,value);
        return index >= 0 ? index : Math.Max(0,~index-1);
    }
    public void Read(int index, float[] pose)
    {
        if (index < 0 || index >= times.Length || pose.Length != Paths.Length*7) throw new ArgumentOutOfRangeException();
        reader.BaseStream.Position = offsets[index]+8;
        for (int i=0;i<pose.Length;i++) pose[i] = reader.ReadSingle();
    }
    public DeviceSnapshot SnapshotAt(int frame)
    {
        int index=snapshotFrames.BinarySearch(frame);
        if(index<0)index=~index-1;
        return index<0?null:snapshots[index];
    }
    public void Dispose() { if (reader != null) { reader.Dispose(); reader=null; } }
}
