#include "tarinstall.h"
#include <archive.h>
#include <archive_entry.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#define __USE_GNU 1
#include <string.h>

#define DESKTOP_HEADER "[Desktop Entry]\nVersion=1.0\nType=Application\nName="
#define DESKTOP_HEADER_LEN sizeof(DESKTOP_HEADER) - 1
#define EXEC_HEADER "\nExec="
#define EXEC_HEADER_LEN sizeof(EXEC_HEADER) - 1
#define INSTALL_DESKTOP "/usr/share/applications/"
#define INSTALL_DESKTOP_LEN sizeof(INSTALL_DESKTOP) - 1
#define INSTALL_DIR "/usr/share/"
#define INSTALL_DIR_LEN sizeof(INSTALL_DIR) - 1
#define ICON_HEADER "\nIcon="
#define ICON_HEADER_LEN sizeof(ICON_HEADER) - 1
#define NO_TERMINAL "\nTerminal=false\n"
#define NO_TERMINAL_LEN sizeof(NO_TERMINAL) - 1
#define WM_CLASS_HEADER "\nStartupWMClass="
#define WM_CLASS_HEADER_LEN sizeof(WM_CLASS_HEADER) - 1

#define MAX_DESKTOP_LEN 8192
#define MAX_FULLPATH_LEN 1024

#define ARCHIVE_FLAGS ARCHIVE_EXTRACT_TIME | ARCHIVE_EXTRACT_PERM | ARCHIVE_EXTRACT_ACL | ARCHIVE_EXTRACT_FFLAGS
#define BIN_DIR "bin/"
#define DESKTOP_EXTENSION ".desktop"

static int memalphacmp(const void *s1, const void *s2, size_t size)
{
	if (size == 0)
		return 0;
	
	const char *s1_str = (const char *)s1;
	const char *s2_str = (const char *)s2;

	// Compare all alpha characters
	while (--size && (!isalpha(*s1_str) || !isalpha(*s2_str) || *s1_str == *s2_str))
	{
		s1_str++;
		s2_str++;
	}

	return *s1_str - *s2_str;
}

static int app_extract(const char *path, DesktopApp *app)
{
	// Open the archive reader
	struct archive *archive = archive_read_new();

	if (archive == NULL)
	{
		fprintf(stderr, "Error creating archive : cannot allocate structure\n");
		return EXIT_FAILURE;
	}

	// Apply support settings
	archive_read_support_format_tar(archive);
	archive_read_support_filter_gzip(archive);  // For .tar.gz
	archive_read_support_filter_xz(archive);    // For .tar.xz
	archive_read_support_filter_bzip2(archive); // For .tar.bz2

	// Open the archive path
	int ret = archive_read_open_filename(archive, path, 10240);  // Read 10KB blocks
    
	if (ret != ARCHIVE_OK)
	{
		fprintf(stderr, "Error opening archive: %s\n", archive_error_string(archive));
		return EXIT_FAILURE;
	}

	// Pointer for entry
	struct archive_entry *entry;

	// Fullpath buffer
	char full_path[MAX_FULLPATH_LEN] = {0};
	app->bin_size = 0;
	app->icon_size = 0;

	// To compare icon files
	size_t last_icon_name_size = 0;
	off_t last_icon_size = 0;

	// Extract each entry
	while (archive_read_next_header(archive, &entry) == ARCHIVE_OK)
	{
		// Set the target directory
		const char *current_file = archive_entry_pathname(entry);

		int first_entry = *full_path == 0;

		// Create fullpath
		size_t file_name_length = strlen(current_file);
		const char *path_end = mempcpy(mempcpy(full_path, INSTALL_DIR, INSTALL_DIR_LEN), current_file, file_name_length + 1);

		// Define app name if need
		if (first_entry)
		{
			app->name_size = file_name_length - 1;
			*(char*)mempcpy(app->name, current_file, app->name_size) = 0;
			printf("Extracting app : %s\n", app->name);
		}

		// Search app executable
		const char *file_end = strrchr(full_path, '/');
		const char *file_ext = file_end ? strchr(file_end, '.') : NULL;

		if (app->bin_size == 0 && file_end && file_end[1] && !file_ext
			&& memalphacmp(app->name, file_end + 1, path_end - file_end - 2) == 0)
		{
			printf("Found app bin : %s\n", full_path);
			app->bin_size = INSTALL_DIR_LEN + file_name_length;
			mempcpy(app->bin, full_path, app->bin_size + 1);
		}

		// Set archive path
		archive_entry_set_pathname(entry, full_path);

		// Extract the file
		ret = archive_read_extract(archive, entry, ARCHIVE_FLAGS);

		if (ret != ARCHIVE_OK)
		{
			fprintf(stderr, "Error extracting file: %s\n", archive_error_string(archive));
			break;
		}

		// Search for icon file

		// Check if file extension is png or svg
		if (!file_ext || (strcasestr(file_ext, ".png") == NULL && strcasestr(file_ext, ".svg") == NULL))
			continue;
		
		size_t icon_name_size = file_ext - file_end - 1;
		struct stat file_info;

		// Check if app->name starts with file name and name size is bigger than last one (so nearest from real app name)
		if (icon_name_size < last_icon_name_size || memalphacmp(app->name, file_end + 1, file_ext - file_end - 1))
			continue;

		ret = stat(full_path, &file_info);

		// Get file stat for size info
		if (ret != ARCHIVE_OK)
		{
			perror("stat");
			break;
		}

		// Check if name is bigger / file size is bigger (for better icon quality)
		if (icon_name_size <= last_icon_name_size && file_info.st_size <= last_icon_size)
			continue;

		printf("Found app icon : %s\n", full_path);
		last_icon_size = file_info.st_size;
		last_icon_name_size = icon_name_size;
		app->icon_size = INSTALL_DIR_LEN + file_name_length;
		mempcpy(app->icon, full_path, app->icon_size + 1);
	}

	return archive_read_free(archive) || ret || !app->bin_size;
}

static void *memplowercpy(void *dst, const void *src, size_t size)
{
	char *dst_str = (char *)dst;
	const char *src_str = (const char *)src;

	// Copy src to dst in lower mode
	while (size--)
		*dst_str++ = tolower(*src_str++);
	
	return dst_str;
}

static int app_config(DesktopApp *app)
{
	// Get Desktop file Path
	char desktop_path[MAX_PATH_LEN];

	const char *bin_name = strrchr(app->bin, '/') + 1;
	size_t bin_name_size = app->bin_size - (bin_name - app->bin);

	mempcpy(mempcpy(mempcpy(desktop_path, INSTALL_DESKTOP, INSTALL_DESKTOP_LEN),
		bin_name, bin_name_size), DESKTOP_EXTENSION, sizeof(DESKTOP_EXTENSION));

	// Create Desktop file Content
	char desktop_content[MAX_DESKTOP_LEN];

	size_t app_name_size = app->name_size - (app->name[app->name_size - 1] == '/');

	const char *desktop_end = mempcpy(mempcpy(mempcpy(mempcpy(mempcpy(memplowercpy(mempcpy(mempcpy(mempcpy(desktop_content, DESKTOP_HEADER, DESKTOP_HEADER_LEN),
		app->name, app_name_size), WM_CLASS_HEADER, WM_CLASS_HEADER_LEN), bin_name, bin_name_size), EXEC_HEADER, EXEC_HEADER_LEN), app->bin, app->bin_size),
		ICON_HEADER, ICON_HEADER_LEN), app->icon, app->icon_size), NO_TERMINAL, NO_TERMINAL_LEN);

	printf("Creating Desktop File : %s\n", desktop_path);

	// Create file & copy content
	int fd = open(desktop_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);

	if (fd == -1)
	{
		perror("open");
		return EXIT_FAILURE;
	}

	ssize_t written = write(fd, desktop_content, desktop_end - desktop_content);

	if (written == -1)
		perror("write");

	int ret = close(fd);
	
	if (ret == -1)
		perror("close");

	return written == -1 || ret == -1;
}

int	main(int argc, const char *argv[])
{
	if (argc == 1)
	{
		fprintf(stderr, "%s: missing path\n", argv[0]);
		return EXIT_FAILURE;
	}

	DesktopApp app;
	int ret = EXIT_SUCCESS;
	
	while (--argc)
	{
		if (app_extract(*++argv, &app) != EXIT_SUCCESS || app_config(&app) != EXIT_SUCCESS)
			ret = EXIT_FAILURE;
	}

	return ret;
}
