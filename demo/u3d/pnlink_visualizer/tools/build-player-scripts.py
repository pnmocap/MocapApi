"""Compile script-only changes against an unchanged, matching Unity player.

No scenes/assets/player engine are rebuilt by this tool. Use Unity 6000.0.31f1
for scene or asset changes. Existing output directories are never overwritten.
"""
import argparse
import hashlib
import json
from pathlib import Path
import shutil
import subprocess
import importlib.util
import struct
import os


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--base-player', type=Path, required=True)
    ap.add_argument('--output', type=Path, required=True)
    ap.add_argument('--compiler', type=Path, default=Path(os.environ.get('CSC_EXE',shutil.which('csc') or 'csc.exe')))
    ap.add_argument('--extra-test-source', type=Path)
    ap.add_argument('--test-initializer', default='CalibrationGuideRuntimeProbe')
    ap.add_argument('--version', help='Must match version.json; defaults to that version')
    ap.add_argument('--base-bundle-version', default='0.1.0')
    args = ap.parse_args()
    project = Path(__file__).resolve().parents[1]
    spec=importlib.util.spec_from_file_location('sync_version',project/'tools/sync-version.py')
    version_module=importlib.util.module_from_spec(spec);spec.loader.exec_module(version_module)
    version=version_module.synchronize(project)
    if args.version is not None: assert args.version==version['version'], '--version must match version.json'
    args.version=version['version']
    for pathkey,hashkey in [('binding','binding_sha256'),('windows_x64','windows_x64_sha256')]:
        assert digest(project/version['sdk'][pathkey])==version['sdk'][hashkey], 'Pinned SDK files changed'
    base = args.base_player.resolve()
    output = args.output.resolve()
    assert not output.exists(), 'Preserve existing builds; choose a new output path'
    managed = base / 'visualizer_Data/Managed'
    assert (base / 'visualizer.exe').is_file() and (managed / 'Assembly-CSharp.dll').is_file()
    assert '6000.0.31f1' in (project / 'ProjectSettings/ProjectVersion.txt').read_text()
    # For this change the runtime-generated UI needs no new serialized assets.
    scripts = sorted((project / 'Assets').rglob('*.cs'))
    output.mkdir(parents=True)
    build = output / 'build-evidence'
    build.mkdir()
    dll = build / 'Assembly-CSharp.dll'
    flags = ['/nologo', '/utf8output', '/target:library', '/langversion:latest', '/nostdlib+',
             '/unsafe+', '/optimize+', '/deterministic+',
             '/define:UNITY_STANDALONE_WIN;UNITY_STANDALONE;UNITY_64;UNITY_5_3_OR_NEWER;UNITY_6000_0_OR_NEWER',
             '/out:' + str(dll)]
    flags += ['/reference:' + str(x) for x in sorted(managed.glob('*.dll')) if x.name != 'Assembly-CSharp.dll']
    flags += [str(x) for x in scripts]
    if args.extra_test_source:
        flags.append(str(args.extra_test_source.resolve()))
    rsp = build / 'compile.rsp'
    rsp.write_text('\n'.join('"' + x + '"' for x in flags) + '\n', encoding='utf-8')
    r = subprocess.run([str(args.compiler), '@' + str(rsp)], stdout=subprocess.PIPE,
                       stderr=subprocess.STDOUT, encoding='utf-8', errors='replace')
    (build / 'compile.log').write_text(r.stdout, encoding='utf-8')
    if r.returncode:
        print(r.stdout)
        raise SystemExit(r.returncode)
    player = output / 'player'
    shutil.copytree(base, player)
    installed = player / 'visualizer_Data/Managed/Assembly-CSharp.dll'
    shutil.copyfile(dll, installed)
    # The matched Unity player stores Application.version in a length-prefixed UTF-8 field.
    # Equal-width replacement preserves every serialized offset; other version lengths need Unity Editor.
    metadata=player/'visualizer_Data/globalgamemanagers'
    before=metadata.read_bytes(); old=args.base_bundle_version.encode();new=version['version'].encode()
    old_field=struct.pack('<I',len(old))+old;new_field=struct.pack('<I',len(new))+new
    bundle_update={'file':'visualizer_Data/globalgamemanagers','version':version['version'],'changed':False}
    if before.count(new_field)!=1 or old!=new and before.count(old_field)==1:
        assert len(old)==len(new) and before.count(old_field)==1, 'Build with Unity Editor for a changed-length or unknown bundleVersion'
        offset=before.index(old_field)+4
        after=before[:offset]+new+before[offset+len(old):]
        assert len(after)==len(before)
        metadata.write_bytes(after)
        bundle_update.update(changed=True,offset=offset,previous=args.base_bundle_version,
                             before_sha256=hashlib.sha256(before).hexdigest(),after_sha256=digest(metadata))
    if args.extra_test_source:
        initializers = player / 'visualizer_Data/RuntimeInitializeOnLoads.json'
        data = json.loads(initializers.read_text(encoding='utf-8'))
        data['root'].append({'assemblyName': 'Assembly-CSharp', 'nameSpace': '',
                             'className': args.test_initializer, 'methodName': 'Install',
                             'loadTypes': 0, 'isUnityClass': True})
        initializers.write_text(json.dumps(data, separators=(',', ':')), encoding='utf-8')
    manifest = {
        'version': args.version, 'build_number':version['build'],'tag':version['tag'],'sdk':version['sdk'],'bundle_version_update':bundle_update, 'build_method': 'script-only compilation against a matching Unity player; equal-width bundleVersion metadata update',
        'unity_version': '6000.0.31f1', 'test_harness_included': bool(args.extra_test_source),
        'source_files': {x.relative_to(project).as_posix(): digest(x) for x in scripts},
        'base_player': str(base), 'base_script_assembly_sha256': digest(managed / 'Assembly-CSharp.dll'),
        'script_assembly_sha256': digest(installed),
        'compiler': str(args.compiler),
        'reference_assemblies': {x.name: digest(x) for x in sorted(managed.glob('*.dll')) if x.name != 'Assembly-CSharp.dll'},
        'player_files': {x.relative_to(player).as_posix(): digest(x) for x in sorted(player.rglob('*')) if x.is_file()},
        'device_or_algorithm_modified': False,
    }
    (output / 'manifest.json').write_text(json.dumps(manifest, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
    print(json.dumps({'passed': True, 'scripts': len(scripts), 'output': str(output),
                      'script_assembly_sha256': digest(installed), 'test_harness': bool(args.extra_test_source)}, ensure_ascii=False))


if __name__ == '__main__':
    main()
