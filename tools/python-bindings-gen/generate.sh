#!/bin/bash

SCRIPT_PATH="$(cd "$(dirname "$0")" && pwd)"

if [ $# -lt 2 ]; then
    echo "Snn-library bindings Python generator" >&2
    echo -e "Usage:\n    $(basename $0) <snn-library-path> <output-path>" >&2
    echo >&2
    exit 1 >&2
fi

IN_PATH="$1"
OUT_PATH="$2"

shift 2

PYBIND11_GEN_BIN="${SCRIPT_PATH}/py-gen/boost_python_gen.py"

mkdir -p "${OUT_PATH}" || exit 1

if [ ! -z "$(ls -A ${OUT_PATH})" ]; then
    echo "\"${OUT_PATH}\" directory is not empty!" >&2
    exit 1
fi

echo "Generating..."

for in_dir in $(find "${IN_PATH}" -type d); do
    echo "Processing ${in_dir}"
    files=$(find "${in_dir}" -maxdepth 1 -type f -iname \*.h)
    target_file_name=${in_dir##${IN_PATH}}

    if [ -z "${files}" -o -z "${target_file_name}" ]; then
        echo "No files..."
        continue
    fi

    target_file_name="${target_file_name##/}"
    class_name="$(echo "${target_file_name}"|sed -e "s/[\/_-]/ /g;s/\b\(.\)/\u\1/g;s/ //g")"
    target_file_name="${target_file_name//[\/-]/_}.cpp"

    echo "${class_name} -> ${target_file_name}"
    python3 "${PYBIND11_GEN_BIN}" "${class_name}" $* ${files} > "${OUT_PATH}/${target_file_name}"
done
