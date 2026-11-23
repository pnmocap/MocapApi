using UnityEngine;

public abstract class AbstractCommandProcess
{
  ulong _command;
  string _title;
  protected AbstractCommandProcess(ulong command, string title) {
    _command = command;
    _title = title;
  }

  public string title { get { return _title; } }
  public ulong command {
    get { return _command; }
  }
  public void DestroyCommand() {
    if (_command == 0) return;
    MocapApi.IMCPCommand.Command.DestroyCommand(_command);
    _command = 0;
  }

  // A response is a normal acknowledgement, including for commands without progress UI.
  public virtual void handleResponse() { }
  public virtual void handleInterrupted(string reason) { DestroyCommand(); }
  public abstract void handleRunning();

  public abstract void handleResult();

  public abstract bool isComplete();


}
