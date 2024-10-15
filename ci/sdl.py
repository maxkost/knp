#!/usr/bin/env python3

"""Helpers for SDL process."""
import json
import os
from pathlib import Path
import re
import urllib.request
from urllib.parse import urlparse, urlunparse, ParseResult
from typing import Any

ARTIFACTS_URL = os.getenv('ARTIFACTS_URL')
SDL_ARTIFACTS_DIRECTORY = os.getenv('SDL_ARTIFACTS_DIRECTORY', 'build/SDL')

HLA_FILENAME = 'hla.xml'

THIRD_PARTY_FILENAME = 'third_party.xml'
THIRD_PARTY_LIST_FILENAME = '3d-party-requirements.xml'

SECURE_CODE_REVIEW_FILENAME = 'code_review.xml'
DEVOPS_PIPELINE_FILENAME = 'devops_pipeline.xml'
STATIC_ANALYSIS_FILENAME = 'static_analysis.xml'
DYNAMIC_ANALYSIS_FILENAME = 'dynamic_analysis.xml'

BUILD_NUMBER = os.getenv('BUILD_NUMBER')

# TFS ID of the release card.
RELEASE_NUMBER = os.getenv('TFS_RELEASE_NUMBER')
COLLECTION_URI = os.getenv('COLLECTION_URI')

KNP_ROOT = Path(__file__).resolve().parent.parent


def artifact_url(
    file_name: str, build: str = BUILD_NUMBER, server: str = ARTIFACTS_URL, add_build_to_name: bool = True
) -> str:
    return f'{server}/{build}/SDL/{build}_{file_name}' if add_build_to_name else f'{server}/{build}/SDL/{file_name}'


def generate_tfs_url(branch: str = 'develop', project: str = 'FT-SNN', repo: str = 'KNP', path: str = '/') -> str:
    url_comps = urlparse(f'{COLLECTION_URI}/{project}/_git/{repo}?path={path}&amp;version=GB{branch}')

    return urlunparse(
        ParseResult(
            url_comps.scheme,
            url_comps.netloc,
            str(Path(url_comps.path)),
            url_comps.params,
            url_comps.query,
            url_comps.fragment,
        )
    )


def generate_hla_artifact_xml() -> str:
    return '''<SDL>
    <hla>
        <hla name="not_applicable" description="This is a platform, approved by Product Security" />
    </hla>
</SDL>'''


def generate_third_party_xml(third_party_url: str) -> str:
    return f'''<SDL>
    <third_party>
        <third_party link="{artifact_url(third_party_url)}"/>
    </third_party>
</SDL>'''


def generate_devops_pipeline_xml() -> str:
    return f'''<SDL>
    <devops_pipelines>
        <devops_pipeline name="{RELEASE_NUMBER}" trusted_environment="false"/>
    </devops_pipelines>
</SDL>'''


def generate_code_review_xml(link: str) -> str:
    return f'''<SDL>
    <code_reviews>
        <code_review
            type="git"
            link="{link}"
        />
    </code_reviews>
</SDL>'''


def get_config(filename: Path, regex: str) -> str:
    with open(filename, encoding='utf8') as config_file:
        file_content = config_file.read()
        m_res = re.findall(regex, file_content, re.MULTILINE | re.IGNORECASE)
        if not m_res:
            raise RuntimeError(f'"{filename}" doesn\'t contain configuration data!')
        return re.sub(r'\s\s+', '\n', '\n'.join(m_res))


def get_pvs_config(filename: Path = KNP_ROOT / 'knp' / 'CMakeLists.txt') -> str:
    return get_config(filename, r'pvs_studio_add_target\(([^\)]+)\)')


def get_valgrind_config(filename: Path = KNP_ROOT / 'knp' / 'tests' / 'CMakeLists.txt') -> str:
    return get_config(filename, r'set\((CTEST_MEMORYCHECK_COMMAND[^\)]+)\)')


def generate_static_analysis_xml() -> str:
    pvs_logs = []

    for osname in ['linux', 'windows']:
        pvs_logs_archive = f'{osname}_pvs_report.7z'
        pvs_config = f'{BUILD_NUMBER}_{osname}_pvs_config.txt'

        with open(KNP_ROOT / SDL_ARTIFACTS_DIRECTORY / pvs_config, 'w', encoding='utf8') as pc_f:
            pc_f.write(get_pvs_config())

        pvs_logs.append(
            f'<analyzer name="PVS Studio C++ {osname.capitalize()}" type="pvs">'
            f'<log link="{artifact_url(pvs_logs_archive)}"/>'
            f'<config name="CMakeLists.txt" link="{artifact_url(pvs_config)}"/>'
            '</analyzer>'
        )

    return f'''<SDL>
    <static_analysis>
        {' '.join(pvs_logs)}
        <analyzer name="OCLint C++ Linux" type="oclint">
            <config name=".oclint" link="{artifact_url('oclint')}"/>
            <log link="{artifact_url('linux_oclint_report.7z')}"/>
        </analyzer>
        <analyzer name="PyLint Linux" type="pylint">
            <log link="{artifact_url('linux_pylint_report.7z')}"/>
            <config name=".pylintrc" link="{artifact_url('pylintrc')}"/>
        </analyzer>
        <analyzer name="Bandit Python Linux" type="bandit">
            <log link="{artifact_url('linux_bandit_report.7z')}"/>
        </analyzer>
    </static_analysis>
</SDL>'''


def generate_dynamic_analysis_xml() -> str:
    with open(KNP_ROOT / SDL_ARTIFACTS_DIRECTORY / f'{BUILD_NUMBER}_valgrind_config.txt', 'w', encoding='utf8') as pc_f:
        pc_f.write(get_valgrind_config())

    return f'''<SDL>
    <dynamic_analysis>
        <analyzer name="Valgrind Linux" type="valgrind">
            <log link="{artifact_url('linux_dynamic_testing_report.7z')}"/>
            <config name="CMakeLists.txt" link="{artifact_url('valgrind_config.txt')}"/>
        </analyzer>
    </dynamic_analysis>
</SDL>'''


def generate_sdl_artifacts() -> None:
    sdl_path = KNP_ROOT / SDL_ARTIFACTS_DIRECTORY
    sdl_path.mkdir(parents=True, exist_ok=True)

    print(f'Generating SDL XMLs for build {BUILD_NUMBER} in path "{sdl_path}"...')

    with open(sdl_path / HLA_FILENAME, 'w', encoding='utf8') as f:
        f.write(generate_hla_artifact_xml())

    with open(sdl_path / THIRD_PARTY_FILENAME, 'w', encoding='utf8') as f:
        f.write(generate_third_party_xml(THIRD_PARTY_LIST_FILENAME))

    with open(sdl_path / DEVOPS_PIPELINE_FILENAME, 'w', encoding='utf8') as f:
        f.write(generate_devops_pipeline_xml())

    with open(sdl_path / SECURE_CODE_REVIEW_FILENAME, 'w', encoding='utf8') as f:
        f.write(generate_code_review_xml(generate_tfs_url()))

    with open(sdl_path / STATIC_ANALYSIS_FILENAME, 'w', encoding='utf8') as f:
        f.write(generate_static_analysis_xml())

    with open(sdl_path / DYNAMIC_ANALYSIS_FILENAME, 'w', encoding='utf8') as f:
        f.write(generate_dynamic_analysis_xml())


def generate_sdl_request() -> dict[str, str | dict[str, str]]:
    # major, minor, *_ = RELEASE_NUMBER.split('.')
    release_iteration = r'FT-SNN\Open Source Release'
    return {
        'release_tfs_id': RELEASE_NUMBER,
        'build_number': BUILD_NUMBER,
        'hla': artifact_url(HLA_FILENAME, add_build_to_name=False),
        'third_party': {'type': 'uri', 'data': artifact_url(THIRD_PARTY_FILENAME, add_build_to_name=False)},
        'devops_pipeline': {
            'type': 'uri',
            'data': artifact_url(DEVOPS_PIPELINE_FILENAME, add_build_to_name=False),
        },
        'source_code': {
            'type': 'uri',
            'data': artifact_url(SECURE_CODE_REVIEW_FILENAME, add_build_to_name=False),
        },
        'static_analysis': {
            'type': 'uri',
            'data': artifact_url(STATIC_ANALYSIS_FILENAME, add_build_to_name=False),
        },
        'dynamic_analysis': {
            'type': 'uri',
            'data': artifact_url(DYNAMIC_ANALYSIS_FILENAME, add_build_to_name=False),
        },
        'iteration_path_for_bug': release_iteration,
        'iteration_path_for_req': release_iteration,
    }


def call_api(url: str, body: dict[str, str | dict[str, str]]) -> Any:
    req = urllib.request.Request(
        url=url,
        data=json.dumps(body).encode('utf8'),
        headers={'Content-Type': 'application/json'},
    )
    return urllib.request.urlopen(req)


def call_vulnbot_api(make_call: bool) -> None:
    sdl_request = generate_sdl_request()
    sdl_api_url = os.getenv('SDL_API_URL')
    print(f'Calling SDL API: {sdl_api_url}\n{sdl_request}')
    if make_call:
        resp = call_api(sdl_api_url, sdl_request)
        print(f'Returned code: {resp.getcode()}')


if '__main__' == __name__:
    import argparse

    parser = argparse.ArgumentParser()
    parser.add_argument('--generate-artifacts', action='store_true', help='Generate SDL artifacts')
    parser.add_argument('--call-api', action='store_true', help='Call Vulnbot API')
    parser.add_argument('--imitate-call', action='store_true', help='Don\'t call Vulnbot API, only imitate call')
    args = parser.parse_args()

    # RELEASE_NUMBER = get_release_number()
    mode_set: bool = False

    if args.generate_artifacts:
        generate_sdl_artifacts()
        mode_set = True
    if args.call_api:
        call_vulnbot_api(not args.imitate_call)
        mode_set = True
    if not mode_set:
        raise ValueError('Set options!')
