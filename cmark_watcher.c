#include "cmark.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <sys/stat.h>

void check_usage(int argc, const char* argv[])
{
  if (argc != 3) {
    printf("%s <markdown_file> <output_html_file>\n", argv[0]);
    exit(1);
  }
}

char* convert_file_to_html_string(const char* arg)
{
  char* markdown_string = NULL;
  long file_length;
  bool no_errors = true;

  FILE* markdown_file = fopen(arg, "rb");

  if (no_errors && !markdown_file) {
    perror("Error opening markdown file");
    no_errors = false;
  }

  if (no_errors && fseek(markdown_file, 0, SEEK_END) == -1) {
    perror("Error seeking end of markdown file");
    no_errors = false;
  }

  if (no_errors) {
    file_length = ftell(markdown_file);
    if (file_length == -1) {
      perror("Error getting length of markdown file");
      no_errors = false;
    }
  }

  if (no_errors && fseek(markdown_file, 0, SEEK_SET) == -1) {
    perror("Error seeking start of markdown file");
    no_errors = false;
  }
  
  if (no_errors) {
    markdown_string = malloc(file_length + 1);
    if (!markdown_string) {
      perror("Error allocating string for markdown file contents");
      no_errors = false;
    }
  }

  if (no_errors) {
    const size_t num_items = 1;
    if (fread(markdown_string, file_length, num_items, markdown_file) < num_items) {
      if (feof(markdown_file)) {
        fprintf(stderr, "Unexpected EOF when reading markdown file\n");
        no_errors = false;
      } else if (ferror(markdown_file)) {
        perror("Error reading from markdown file");
        no_errors = false;
      }
    } else {
      markdown_string[file_length] = '\0';
    }
  }

  char* html_string = NULL;
  if (no_errors)
    html_string =
        cmark_markdown_to_html(markdown_string, file_length, CMARK_OPT_DEFAULT);

  // Cleanup
  if (markdown_file)
    fclose(markdown_file);
  return html_string;
}

void write_html_to_file(const char* html, const char* arg)
{
  bool no_errors = true;

  FILE* html_file = fopen(arg, "wb");

  if (no_errors && !html_file) {
    perror("Error opening output file");
    no_errors = false;
  }

  if (no_errors) {
    const size_t num_items = 1;
    if (fwrite(html, strlen(html), num_items, html_file) < num_items) {
      if (feof(html_file)) {
        fprintf(stderr, "Unexpected EOF when writing html file\n");
        no_errors = false;
      } else if (ferror(html_file)) {
        perror("Error writing to html file");
        no_errors = false;
      }
    }
  }

  if (html_file)
    fclose(html_file);
}

// Poor person's version of inotify. The issue is that inotify
// isn't cross-platform. In practice this seems to work pretty well.
bool file_is_modified(const char *path, time_t* mtime)
{
	struct stat file_stat;
	if (stat(path, &file_stat) == -1) {
			perror("Error checking markdown modification time");
      return false;
	}

  const time_t old_mtime = *mtime;
  if (file_stat.st_mtime > old_mtime) {
    *mtime = file_stat.st_mtime;
    return true;
  }
  return false;
}

int main(int argc, const char* argv[])
{
  check_usage(argc, argv);
  const char* md_path = argv[1];
  const char* html_path = argv[2];

  char* html = NULL;
  time_t mtime = 0;

  while (true) {
    if (file_is_modified(md_path, &mtime)) {
      html = convert_file_to_html_string(md_path);
      if (html) {
        write_html_to_file(html, html_path);
        free(html);
        html = NULL;
      }
    }
    usleep(20000);
  }
}
