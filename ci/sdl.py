#!/usr/bin/env python3

"""Helpers for SDL process."""
import json
import os
import pathlib
import urllib.request
from typing import Any

ARTIFACT_HOSTNAME = os.getenv('SDL_ARTIFACT_HOSTNAME')

STATIC_ANALYSIS_FILENAME = 'static_analysis.xml'
DYNAMIC_ANALYSIS_FILENAME = 'dynamic_analysis.xml'
SECURE_CODE_REVIEW_FILENAME = 'secure_code_review.xml'
THIRD_PARTY_FILENAME = 'third_party.xml'
THREAT_MODELING_FILENAME = 'threat_modeling.xml'


def artifact_url(file_name: str, build: str, server: str = ARTIFACT_HOSTNAME) -> str:
    return f'http://{server}/tfs_artifacts/{build}/{file_name}'


def generate_hla_artefact_xml(hla_link: str) -> str:
    return f'''<SDL>
    <hla link="{hla_link}" />
</SDL>'''


def generate_code_review_xml(branch: str = 'develop', project: str = 'KNP', path: str = '/') -> str:
    return f'''<SDL>
    <code_reviews>
        <code_review
            type="git"
            link="https://hqrndtfs.avp.ru/tfs/DefaultCollection/_git/{project}?path={path}&amp;version=GB{branch}"
        />
    </code_reviews>
</SDL>'''


def generate_static_analysis_xml(build: str) -> str:
    pylint_config_url = artifact_url('.pylintrc', build)
    pylint_report_url = artifact_url('pylint-report-release.tar.gz', build)
    return f'''<SDL>
    <static_analysis>
        <analyzer name="pylint">
            <config name="pylintrc" link="{pylint_config_url}"/>
            <log link="{pylint_report_url}"/>
        </analyzer>
    </static_analysis>
</SDL>'''


def generate_dynamic_analysis_xml(_build: str) -> str:
    return '''<SDL>
    <dynamic_analysis>
        <analyzer
            name="not_applicable"
            type=""
            version=""
            description="no dynamic analyzer for JavaScript and Python"
        />
    </dynamic_analysis>
</SDL>'''


def generate_third_party_xml(build: str) -> str:
    third_party_url = artifact_url('3d-party-requirements.xml', build)
    return f'''<SDL>
    <third_party link="{third_party_url}"/>
</SDL>'''


def generate_sdl_artifacts() -> None:
    build = f'{os.getenv("TFS_PIPELINE")}/{os.getenv("KNP_BUILD_NUMBER")}'
    sdl_path = pathlib.Path(__file__).resolve().parent / 'SDL'
    print('Generating SDL XMLs for build:', build)
    with open(sdl_path / SECURE_CODE_REVIEW_FILENAME, 'w', encoding='utf8') as f:
        f.write(generate_code_review_xml())
    with open(sdl_path / STATIC_ANALYSIS_FILENAME, 'w', encoding='utf8') as f:
        f.write(generate_static_analysis_xml(build))
    with open(sdl_path / DYNAMIC_ANALYSIS_FILENAME, 'w', encoding='utf8') as f:
        f.write(generate_dynamic_analysis_xml(build))
    with open(sdl_path / THIRD_PARTY_FILENAME, 'w', encoding='utf8') as f:
        f.write(generate_third_party_xml(build))
    with open(sdl_path / THREAT_MODELING_FILENAME, 'w', encoding='utf8') as f:
        f.write(generate_hla_artefact_xml(hla_link=os.getenv('SDL_THREAT_MODEL_ADDRESS')))


def generate_sdl_request() -> dict[str, str | dict[str, str]]:
    build = '{os.getenv("TFS_PIPELINE")}/{os.getenv("KNP_BUILD_NUMBER")}'
    version = os.getenv('KNP_VERSION')
    major, minor, *_ = version.split('.')
    release_iteration = rf'FT-SNN\KNP {major}.{minor} Release'
    return {
        'release_tfs_id': os.getenv('KNP_RELEASE_NUMBER'),
        'build_number': os.getenv('KNP_BUILD_NUMBER'),
        'hla': artifact_url(THREAT_MODELING_FILENAME, build),
        'source_code': {'type': 'uri', 'data': artifact_url(SECURE_CODE_REVIEW_FILENAME, build)},
        'third_party': {'type': 'uri', 'data': artifact_url(THIRD_PARTY_FILENAME, build)},
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


def call_vulnbot_api() -> None:
    sdl_request = generate_sdl_request()
    sdl_api_url = os.getenv('SDL_API_URL')
    print(f'Calling SDL API: {sdl_api_url}\n{sdl_request}')
    resp = call_api(sdl_api_url, sdl_request)
    print(f'Returned code: {resp.getcode()}')


if '__main__' == __name__:
    import argparse

    parser = argparse.ArgumentParser()
    parser.add_argument('--generate-artifacts', action='store_true', help='Generate SDL artifacts')
    parser.add_argument('--call-api', action='store_true', help='Call Vulnbot API')
    args = parser.parse_args()
    if args.generate_artifacts:
        generate_sdl_artifacts()
    if args.call_api:
        call_vulnbot_api()
