# Kaspersky Neuromorphic Platform contributing guide

# Version control

- In the internal TFS, modified Gitflow is used.
- On Github, standard Github process is used.

In the end of the commit message, issue or task number must be specified after the hash (`#`) symbol.

Branches:

- master - release branch.
- develop - develop branch.
- feature/\* - feature branches. Split from develop. Name format: `feature/#<issue_number>-branch-name>`. Example: `feature/#123-some-feature`.
- bugfix/\* - error-fixing branches. Similar to feature branches.
- release/\* - release branches. Split from develop, merged into master. Marked with release number tag. Example: `release/1.0.0`.

# Coding Style

- Python: [PEP8](https://www.python.org/dev/peps/pep-0008/) + [Black](https://github.com/psf/black).
- C++: Modified [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html).

In the C++ code, Doxygen docstrings MUST be used.
