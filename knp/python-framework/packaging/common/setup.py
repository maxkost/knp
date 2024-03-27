import pathlib

from setuptools import setup

here = pathlib.Path(__file__).parent.resolve()

# requirements = [r for r in (here / 'requirements.txt').read_text(encoding='utf-8').split()
#                 if r and not r.lstrip().startswith('#')]

setup(
    # install_requires=requirements,
    tests_require=['pytest==7.2.2'],
    # package_data={'knp': [str(so_path) for so_path in pathlib.Path('.').glob('**/*.so')]}
    # scripts=['markdown_tool.py'],
    # entry_points={
    #     'console_scripts': [
    #         'markdown_tool = markdown_tool:main',
    #     ],
    # }
)
