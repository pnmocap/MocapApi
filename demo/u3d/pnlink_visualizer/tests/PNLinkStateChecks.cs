using System;
using System.IO;
public static class PNLinkStateChecks
{
    static int checks;
    static void Check(bool ok,string text){if(!ok)throw new Exception(text);++checks;}
    public static int Main(string[] args)
    {
        try {
            string root=Path.GetFullPath(args[0]);Directory.CreateDirectory(root);
            if(SoloStateChecks.Main(new[]{Path.Combine(root,"legacy")})!=0)return 1;
            var a=new DeviceSnapshot{Available=true,Online=true,Motion=true,Serial="DEVICE-A",Firmware="4.0",Sensors=new[]{new SensorSnapshot{Id=8,Bone="RightArm",Serial="测试序列",Firmware="1.0",PacketRate=97.5f,Magnetic=3}}};
            var b=new DeviceSnapshot{Available=true,Online=false,Motion=false,Serial="DEVICE-B",Sensors=new SensorSnapshot[0]};
            byte[] encoded=a.Encode();var decoded=DeviceSnapshot.Decode(encoded);
            Check(decoded.Serial==a.Serial&&decoded.Sensors[0].Serial=="测试序列"&&decoded.Sensors[0].PacketRate==97.5f,"bounded UTF8 status roundtrip");
            string path=Path.Combine(root,"status-v2.mocap");var pose=new float[]{1,2,3,0,0,0,1};
            using(var w=new MotionRecordingWriter(path,new[]{""},true)){w.Write(0,pose,encoded);w.Write(.5,pose);w.Write(1,pose,b.Encode());}
            using(var r=new MotionRecordingReader(path)) {
                Check(r.Version==2&&r.Frames==3,"version2 indexing");
                Check(r.SnapshotAt(0).Serial=="DEVICE-A"&&r.SnapshotAt(1).Serial=="DEVICE-A"&&r.SnapshotAt(2).Serial=="DEVICE-B","snapshots follow recorded frame and carry forward only within the take");
                Check(r.SnapshotAt(r.FrameAt(.7)).Online&&!r.SnapshotAt(r.FrameAt(1)).Online,"seeking changes recorded online state");
            }
            byte[] bytes=File.ReadAllBytes(path);var damaged=(byte[])bytes.Clone();
            Array.Copy(BitConverter.GetBytes(int.MaxValue),0,damaged,52,4);
            string bad=Path.Combine(root,"bad-v2.mocap");File.WriteAllBytes(bad,damaged);bool rejected=false;
            try{using(var r=new MotionRecordingReader(bad)) {}}catch(InvalidDataException){rejected=true;}
            Check(rejected,"oversized metadata rejected without allocation");File.Delete(bad);
            var cut=new byte[bytes.Length-1];Array.Copy(bytes,cut,cut.Length);File.WriteAllBytes(bad,cut);
            using(var r=new MotionRecordingReader(bad))Check(r.Recovered&&r.Frames==2&&r.SnapshotAt(1).Serial=="DEVICE-A","incomplete metadata frame recovery");
            File.Delete(bad);
            var guide=new CalibrationGuideState();guide.SetSupportedPoses(new[]{"A-Pose","T-Pose","F-Pose"});
            guide.Report(CalibrationGuidePhase.Capturing,"FPose",79);Check(!guide.IsStandingFinish&&guide.Message.Contains("自然向前走路"),"walking before stop cue");
            guide.Report(CalibrationGuidePhase.Capturing,"FPose",80);Check(guide.IsStandingFinish&&guide.Message.Contains("站稳"),"stand still cue at 80 percent");
            guide.ReportReadIssue();Check(guide.Message.Contains("站稳不动"),"read failure preserves final stillness requirement");
            guide.Report(CalibrationGuidePhase.Capturing,"FPose",99);Check(guide.IsStandingFinish&&guide.Phase==CalibrationGuidePhase.Capturing,"99 percent still requires standing");
            guide.Report(CalibrationGuidePhase.Capturing,"FPose",100);Check(!guide.IsStandingFinish&&guide.Phase==CalibrationGuidePhase.Processing,"free movement only after collection");
            CalibrationGuideStateChecks.Main();Console.WriteLine("PASS "+checks+" additional PN Link status/format checks");return 0;
        }catch(Exception e){Console.WriteLine("FAIL "+e.GetType().Name+": "+e.Message);return 1;}
    }
}
