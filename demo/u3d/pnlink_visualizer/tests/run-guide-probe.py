"""Run only a private probe player, without native device communication."""
import argparse
import json
from pathlib import Path
import subprocess
import time
import winreg


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--build', type=Path, required=True)
    parser.add_argument('--width', type=int, default=1280)
    parser.add_argument('--height', type=int, default=800)
    args = parser.parse_args()
    root = args.build.resolve()
    assert json.loads((root / 'manifest.json').read_text())['test_harness_included']
    out = root / f'runtime-{args.width}x{args.height}'
    out.mkdir(exist_ok=False)
    registry = r'Software\Noitom\visualizer'
    before = {}
    with winreg.OpenKey(winreg.HKEY_CURRENT_USER, registry) as key:
        i = 0
        while True:
            try:
                name, value, kind = winreg.EnumValue(key, i)
            except OSError:
                break
            i += 1
            before[name] = (value, kind)
    start = time.monotonic()
    player = root / 'player'
    process = subprocess.Popen([str(player / 'visualizer.exe'), '-screen-fullscreen', '0',
                                '-screen-width', str(args.width), '-screen-height', str(args.height),
                                '-logFile', str(out / 'Player.log'), '--guide-probe-output', str(out)], cwd=player)
    try:
        code = process.wait(timeout=45)
    except subprocess.TimeoutExpired:
        process.terminate()  # Only this private test process.
        process.wait(timeout=10)
        code = -1
    finally:
        with winreg.OpenKey(winreg.HKEY_CURRENT_USER, registry, 0, winreg.KEY_SET_VALUE) as key:
            existing=[]
            with winreg.OpenKey(winreg.HKEY_CURRENT_USER, registry) as read_key:
                i=0
                while True:
                    try: existing.append(winreg.EnumValue(read_key,i)[0]); i+=1
                    except OSError: break
            for name in existing:
                if name not in before: winreg.DeleteValue(key,name)
            for name, (value, kind) in before.items():
                winreg.SetValueEx(key, name, 0, kind, value)
    result = {'exit_code': code, 'seconds': time.monotonic() - start, 'pid': process.pid,
              'passed': code == 0 and (out / 'passed.json').exists() and not (out / 'failure.txt').exists()}
    if (out / 'passed.json').exists():
        result['probe'] = json.loads((out / 'passed.json').read_text())
    if (out / 'failure.txt').exists():
        result['failure'] = (out / 'failure.txt').read_text(encoding='utf-8')
    (out / 'launch-result.json').write_text(json.dumps(result, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
    print(json.dumps(result, ensure_ascii=False))
    return 0 if result['passed'] else 1


if __name__ == '__main__':
    raise SystemExit(main())
