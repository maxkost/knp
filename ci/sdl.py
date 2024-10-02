#!/usr/bin/env python3

"""Helpers for SDL process."""
import json
import os
import pathlib
import urllib.request
from typing import Any

ARTIFACTS_HOSTNAME = os.getenv('SDL_ARTIFACTS_HOSTNAME')
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


def artifact_url(file_name: str, build: str, server: str = ARTIFACTS_HOSTNAME) -> str:
    return f'http://{server}/tfs_artifacts/{build}/{file_name}'


def generate_tfs_url(branch: str = 'develop', project: str = 'FT-SNN', repo: str = 'KNP', path: str = '/') -> str:
    return f'{COLLECTION_URI}/{project}/_git/{repo}?path={path}&amp;version=GB{branch}'


def generate_hla_artifact_xml() -> str:
    return '''<SDL>
    <hla>
        <hla name="not_applicable" description="This is a platform, approved by Product Security" />
    </hla>
</SDL>'''


def generate_third_party_xml(third_party_url: str) -> str:
    return f'''<SDL>
    <third_party>
        <third_party link="{third_party_url}"/>
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


def generate_static_analysis_xml(build: str) -> str:
    oclint_logs_archive = 'oclint.log'
    pvs_logs_archive = 'PVS-Studio.log'
    artifact_url('.pylintrc', build)
    pylint_report_url = artifact_url('pylint-report/pylint-report.txt', build)
    bandit_report_url = artifact_url('bandit.log', build)

    return f'''<SDL>
    <static_analysis>
        <analyzer name="PVS Studio C++" type="pvs">
            <log link="{artifact_url(pvs_logs_archive, build)}"/>
        </analyzer>
        <analyzer name="OCLint C++" type="oclint">
            <config name="oclint" link="{generate_tfs_url(path='.oclint')}"/>
            <log link="{artifact_url(oclint_logs_archive, build)}"/>
        </analyzer>
        <analyzer name="PyLint" type="pylint">
            <config name="pylintrc" link="{generate_tfs_url(path='knp/.pylintrc')}"/>
            <log link="{pylint_report_url}"/>
        </analyzer>
        <analyzer name="Bandit Python" type="bandit">
            <log link="{bandit_report_url}"/>
        </analyzer>
    </static_analysis>
</SDL>'''


def generate_dynamic_analysis_xml(build: str) -> str:
    valgrind_report_url = artifact_url(f'{build}_valgrind.7z', build)

    return f'''<SDL>
    <dynamic_analysis>
        <analyzer name="Valgrind" type="valgrind">
            <log link="{valgrind_report_url}"/>
        </analyzer>
    </dynamic_analysis>
</SDL>'''


def generate_sdl_artifacts() -> None:
    build = f'{os.getenv("TFS_PIPELINE")}/{BUILD_NUMBER}'
    sdl_path = pathlib.Path(__file__).resolve().parent.parent / SDL_ARTIFACTS_DIRECTORY
    sdl_path.mkdir(parents=True, exist_ok=True)

    print(f'Generating SDL XMLs for build {build} in path "{sdl_path}"')

    with open(sdl_path / HLA_FILENAME, 'w', encoding='utf8') as f:
        f.write(generate_hla_artifact_xml())

    with open(sdl_path / THIRD_PARTY_FILENAME, 'w', encoding='utf8') as f:
        f.write(
            generate_third_party_xml(
                f'{generate_tfs_url(path=str(pathlib.Path(__file__).resolve().parent / THIRD_PARTY_LIST_FILENAME))}'
            )
        )

    with open(sdl_path / DEVOPS_PIPELINE_FILENAME, 'w', encoding='utf8') as f:
        f.write(generate_devops_pipeline_xml())

    with open(sdl_path / SECURE_CODE_REVIEW_FILENAME, 'w', encoding='utf8') as f:
        f.write(generate_code_review_xml(generate_tfs_url()))

    with open(sdl_path / STATIC_ANALYSIS_FILENAME, 'w', encoding='utf8') as f:
        f.write(generate_static_analysis_xml(build))

    with open(sdl_path / DYNAMIC_ANALYSIS_FILENAME, 'w', encoding='utf8') as f:
        f.write(generate_dynamic_analysis_xml(build))


def generate_sdl_request() -> dict[str, str | dict[str, str]]:
    build = f'{os.getenv("TFS_PIPELINE")}/{BUILD_NUMBER}'
    # major, minor, *_ = RELEASE_NUMBER.split('.')
    release_iteration = r'FT-SNN\Open Source Release'
    return {
        'release_tfs_id': RELEASE_NUMBER,
        'build_number': BUILD_NUMBER,
        'hla': artifact_url(HLA_FILENAME, build),
        'third_party': {'type': 'uri', 'data': artifact_url(THIRD_PARTY_FILENAME, build)},
        'devops_pipeline': {'type': 'uri', 'data': artifact_url(DEVOPS_PIPELINE_FILENAME, build)},
        'source_code': {'type': 'uri', 'data': artifact_url(SECURE_CODE_REVIEW_FILENAME, build)},
        'static_analysis': {'type': 'uri', 'data': artifact_url(STATIC_ANALYSIS_FILENAME, build)},
        'dynamic_analysis': {'type': 'uri', 'data': artifact_url(DYNAMIC_ANALYSIS_FILENAME, build)},
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
    work_completed: bool = False

    if args.generate_artifacts:
        generate_sdl_artifacts()
        work_completed = True
    if args.call_api:
        call_vulnbot_api(args.imitate_call)
        work_completed = True
    if not work_completed:
        raise ValueError('Set options!')
