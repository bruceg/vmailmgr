set -e
archive="$1"
shift
tmpdir=".libmerge.$archive.$$.$RANDOM.$USER"
mkdir "$tmpdir"
cd "$tmpdir"
trap 'cd ..; rm -rf "$tmpdir"' EXIT
for input in "$@"; do
	ar x ../"$input"
done
ar rc ../"$archive" *
rm -f *
ranlib ../"$archive"
