using System;
using System.IO;
using System.Text;

// Plain data: safe to serialize and validate off the Unity thread. No native handles are recorded.
public sealed class SensorSnapshot
{
    public uint Id;
    public string Bone="", Serial="", Firmware="";
    public float PacketRate=-1;
    public int Magnetic=-1;
}
public sealed class DeviceSnapshot
{
    public bool Available, Online, Motion;
    public string Serial="", Firmware="";
    public SensorSnapshot[] Sensors=new SensorSnapshot[0];
    public byte[] Encode()
    {
        using(var memory=new MemoryStream()) using(var w=new BinaryWriter(memory)) {
            w.Write(1); w.Write(Available); w.Write(Online); w.Write(Motion);
            WriteString(w,Serial); WriteString(w,Firmware);
            if(Sensors.Length>512)throw new InvalidDataException("Too many sensors");
            w.Write(Sensors.Length);
            foreach(var s in Sensors) { w.Write(s.Id); WriteString(w,s.Bone);WriteString(w,s.Serial);WriteString(w,s.Firmware);w.Write(s.PacketRate);w.Write(s.Magnetic); }
            w.Flush(); return memory.ToArray();
        }
    }
    public static DeviceSnapshot Decode(byte[] bytes)
    {
        using(var memory=new MemoryStream(bytes,false)) using(var r=new BinaryReader(memory)) {
            if(r.ReadInt32()!=1)throw new InvalidDataException("Unsupported device snapshot");
            var d=new DeviceSnapshot { Available=r.ReadBoolean(),Online=r.ReadBoolean(),Motion=r.ReadBoolean(),Serial=ReadString(r),Firmware=ReadString(r) };
            int count=r.ReadInt32(); if(count<0||count>512)throw new InvalidDataException("Invalid sensor count");
            d.Sensors=new SensorSnapshot[count]; var ids=new System.Collections.Generic.HashSet<uint>();
            for(int i=0;i<count;i++) {
                var s=new SensorSnapshot {Id=r.ReadUInt32(),Bone=ReadString(r),Serial=ReadString(r),Firmware=ReadString(r),PacketRate=r.ReadSingle(),Magnetic=r.ReadInt32()};
                if(!ids.Add(s.Id)||float.IsNaN(s.PacketRate)||float.IsInfinity(s.PacketRate)||s.PacketRate < -1 ||s.PacketRate>10000||s.Magnetic < -1||s.Magnetic>3)throw new InvalidDataException("Invalid sensor status");
                d.Sensors[i]=s;
            }
            if(memory.Position!=memory.Length)throw new InvalidDataException("Unexpected device snapshot bytes");
            return d;
        }
    }
    static void WriteString(BinaryWriter w,string value) {
        var b=Encoding.UTF8.GetBytes(value??""); if(b.Length>4096)throw new InvalidDataException("Device text too long"); w.Write(b.Length);w.Write(b);
    }
    static string ReadString(BinaryReader r) {
        int n=r.ReadInt32();if(n<0||n>4096)throw new InvalidDataException("Invalid device text");var b=r.ReadBytes(n);if(b.Length!=n)throw new EndOfStreamException();return new UTF8Encoding(false,true).GetString(b);
    }
}
