#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
MCU="${MCU:-33CK256MP506}"
TOOL="${PROGRAMMER_TOOL:-PK4}"
HEX_FILE="${HEX_FILE:-$ROOT_DIR/out/zp-prd-a-probe-pic-headfull/default.hex}"
POWER_TARGET="${POWER_TARGET:-0}"
DRY_RUN=0

usage() {
    cat <<USAGE
Usage: $(basename "$0") [--hex PATH] [--tool TOOL] [--mcu MCU] [--power] [--dry-run] [extra ipecmd args...]

Environment overrides:
  IPECMD=/path/to/ipecmd.sh
  HEX_FILE=$HEX_FILE
  PROGRAMMER_TOOL=$TOOL
  MCU=$MCU
  POWER_TARGET=1
USAGE
}

require_value() {
    if [ "$#" -lt 2 ]; then
        printf 'Missing value for %s\n' "$1" >&2
        exit 1
    fi
}

find_ipecmd() {
    if [ -n "${IPECMD:-}" ]; then
        printf '%s\n' "$IPECMD"
        return 0
    fi

    local candidate
    for candidate in \
        /opt/microchip/mplabx/*/mplab_platform/mplab_ipe/ipecmd.sh \
        /opt/microchip/mplabx/*/mplab_platform/bin/ipecmd.sh \
        "$HOME"/mplab/mplabx/mplab_platform/mplab_ipe/ipecmd.sh \
        "$HOME"/mplab/mplabx/mplab_platform/bin/ipecmd.sh \
        "$HOME"/mplab/mplabx/*/mplab_platform/mplab_ipe/ipecmd.sh \
        "$HOME"/mplab/mplabx/*/mplab_platform/bin/ipecmd.sh \
        "$HOME"/mplabx/*/mplab_platform/mplab_ipe/ipecmd.sh \
        "$HOME"/.mplab/*/mplab_platform/mplab_ipe/ipecmd.sh
    do
        if [ -x "$candidate" ]; then
            printf '%s\n' "$candidate"
            return 0
        fi
    done

    if command -v ipecmd.sh >/dev/null 2>&1; then
        command -v ipecmd.sh
        return 0
    fi
    if command -v ipecmd >/dev/null 2>&1; then
        command -v ipecmd
        return 0
    fi

    return 1
}

EXTRA_ARGS=()
while [ "$#" -gt 0 ]; do
    case "$1" in
        --hex)
            require_value "$@"
            HEX_FILE="$2"
            shift 2
            ;;
        --hex=*)
            HEX_FILE="${1#*=}"
            shift
            ;;
        --tool)
            require_value "$@"
            TOOL="$2"
            shift 2
            ;;
        --tool=*)
            TOOL="${1#*=}"
            shift
            ;;
        --mcu)
            require_value "$@"
            MCU="$2"
            shift 2
            ;;
        --mcu=*)
            MCU="${1#*=}"
            shift
            ;;
        --power)
            POWER_TARGET=1
            shift
            ;;
        --dry-run)
            DRY_RUN=1
            shift
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        --)
            shift
            EXTRA_ARGS+=("$@")
            break
            ;;
        *)
            EXTRA_ARGS+=("$1")
            shift
            ;;
    esac
done

if [ ! -f "$HEX_FILE" ]; then
    printf 'Missing HEX file: %s\nRun scripts/build.sh first.\n' "$HEX_FILE" >&2
    exit 1
fi

if ! IPECMD_PATH="$(find_ipecmd)"; then
    printf 'Could not find ipecmd. Install MPLAB X IPE or set IPECMD=/path/to/ipecmd.sh\n' >&2
    exit 1
fi

CMD=(
    "$IPECMD_PATH"
    "-TP$TOOL"
    "-P$MCU"
    "-F$HEX_FILE"
    -M
    -OL
)

if [ "$POWER_TARGET" = "1" ]; then
    CMD+=(-W)
fi

CMD+=("${EXTRA_ARGS[@]}")

printf '%q ' "${CMD[@]}"
printf '\n'

if [ "$DRY_RUN" = "1" ]; then
    exit 0
fi

exec "${CMD[@]}"
