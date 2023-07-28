The purpose of this small project is to write a watcher utility that runs the
CommonMark reference html generator whenever the target markdown file is changed.
In this way it should be possible to point a web browser to the local html file
and see how the browser renders the markdown.

BUILDING:
git submodule update --init
mkdir -p build
cd build
cmake ..
make

RUNNING:
The build should produce cmark_watcher, which requires two ordered arguments.
The first is the markdown file path to watch, and the second is the html path to write.
Use ctrl+C to stop the watcher.
