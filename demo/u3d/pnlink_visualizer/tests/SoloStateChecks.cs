using System;
using System.IO;
using System.Threading;

public static class SoloStateChecks
{
    static int checks;
    static void Check(bool ok,string label) { if(!ok)throw new Exception(label); ++checks; }
    static void Rejected(string path,byte[] bytes)
    {
        File.WriteAllBytes(path,bytes); bool rejected=false;
        try { using(var r=new MotionRecordingReader(path)){} } catch(InvalidDataException){rejected=true;} catch(EndOfStreamException){rejected=true;}
        Check(rejected,"Malformed recording accepted");
        File.Delete(path); // Also verifies failed construction released the file handle on Windows.
    }
    public static int Main(string[] args)
    {
        try { return Run(args); }
        catch(Exception e) { Console.WriteLine("FAIL "+e.GetType().Name); Console.WriteLine(e.Message); return 1; }
    }
    static int Run(string[] args)
    {
        string folder=Path.GetFullPath(args[0]); Directory.CreateDirectory(folder);
        string good=Path.Combine(folder,"roundtrip.mocap"), bad=Path.Combine(folder,"bad.mocap");
        var first=new float[]{1,2,3,0,0,0,1,4,5,6,0,0,0,1};
        var last=new float[]{7,8,9,0,0,0,1,10,11,12,0,0,0,1};
        using(var w=new MotionRecordingWriter(good,new[]{"","Bone"})) { w.Write(0,first); w.Write(.2,first); w.Write(.8,last); Check(w.Frames==3,"frame count"); }
        using(var r=new MotionRecordingReader(good)) {
            Check(r.Frames==3 && r.Duration==.8 && !r.Recovered,"versioned recording roundtrip");
            Check(r.FrameAt(-1)==0 && r.FrameAt(.1)==0 && r.FrameAt(.2)==1 && r.FrameAt(100)==2,"seek boundaries");
            var actual=new float[14]; r.Read(2,actual);
            Check(string.Join(",",actual)==string.Join(",",last),"binary float roundtrip exact");
        }
        byte[] bytes=File.ReadAllBytes(good); var damaged=(byte[])bytes.Clone(); damaged[0]=0; Rejected(bad,damaged);
        damaged=(byte[])bytes.Clone(); Array.Copy(BitConverter.GetBytes(int.MaxValue),0,damaged,8,4); Rejected(bad,damaged);
        damaged=(byte[])bytes.Clone(); Array.Copy(BitConverter.GetBytes(int.MaxValue),0,damaged,12,4); Rejected(bad,damaged);
        // Header: 8 magic + 4 count + 4 empty path + 4 length + 4 "Bone" = 24.
        damaged=(byte[])bytes.Clone(); Array.Copy(BitConverter.GetBytes(double.NaN),0,damaged,24,8); Rejected(bad,damaged);
        damaged=(byte[])bytes.Clone(); Array.Copy(BitConverter.GetBytes(-.1),0,damaged,24,8); Rejected(bad,damaged);
        damaged=(byte[])bytes.Clone(); Array.Copy(BitConverter.GetBytes(0d),0,damaged,24+64,8); Rejected(bad,damaged);
        damaged=(byte[])bytes.Clone(); Array.Copy(BitConverter.GetBytes(float.NaN),0,damaged,32,4); Rejected(bad,damaged);
        damaged=(byte[])bytes.Clone(); Array.Copy(BitConverter.GetBytes(0f),0,damaged,32+24,4); Rejected(bad,damaged);
        File.WriteAllBytes(bad,new ArraySegment<byte>(bytes,0,bytes.Length-9).ToArray());
        using(var r=new MotionRecordingReader(bad))Check(r.Frames==2 && r.Recovered,"partial trailing frame recovered");
        File.Delete(bad);
        var cts=new CancellationTokenSource(); cts.Cancel(); bool canceled=false;
        try { using(var r=new MotionRecordingReader(good,cts.Token)){} }catch(OperationCanceledException){canceled=true;}
        Check(canceled,"large-file validation cancellable"); cts.Dispose();
        var guide=new CalibrationGuideState(); guide.SetSupportedPoses(new[]{"A-Pose","T-Pose","F-Pose"});
        guide.Report(CalibrationGuidePhase.Capturing,"T-Pose",100);
        Check(guide.Phase==CalibrationGuidePhase.Capturing && !guide.Message.Contains("自由活动"),"intermediate pose 100 is not all collection complete");
        guide.Report(CalibrationGuidePhase.Capturing,"FPose",99);
        Check(guide.IsWalkingPose && guide.Message.Contains("走路"),"unhyphenated F pose recognized");
        guide.Report(CalibrationGuidePhase.Capturing,"FPose",100);
        Check(guide.Phase==CalibrationGuidePhase.Processing && guide.IsRunning,"processing still awaits device result");
        guide.ReportReadIssue(); Check(guide.Message.Contains("自由活动"),"read error does not resume walking after collection");
        guide.Finish(false,"failed"); Check(guide.Phase==CalibrationGuidePhase.Failed,"processing can end in failure");
        guide=new CalibrationGuideState(); guide.SetSupportedPoses(new[]{"F-Pose","T-Pose"});
        guide.Report(CalibrationGuidePhase.Capturing,"F-Pose",100);
        Check(guide.Phase==CalibrationGuidePhase.Capturing,"custom order does not release before final pose");
        Console.WriteLine("PASS "+checks+" recording and solo workflow checks"); return 0;
    }
    static byte[] ToArray(this ArraySegment<byte> segment) { var a=new byte[segment.Count]; Array.Copy(segment.Array,segment.Offset,a,0,a.Length); return a; }
}
