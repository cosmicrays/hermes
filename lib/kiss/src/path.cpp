#include "kiss/path.h"

#ifdef WIN32
#include "windows.h"
#ifndef S_ISREG
#define S_ISREG(x) (((x) & S_IFMT) == S_IFREG)
#endif
#ifndef S_ISDIR
#define S_ISDIR(x) (((x) & S_IFMT) == S_IFDIR)
#endif
#include "sys/stat.h"
#elif defined(__APPLE__)
#include <dirent.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <mach-o/dyld.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#else
#include <dirent.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>

#include "sys/stat.h"
#endif

#include <cstdio>
#include <stdexcept>

bool create_directory(const std::string &path, size_t user_permission, size_t group_permission,
                      size_t other_permission) {
#ifdef WIN32
	BOOL result = ::CreateDirectoryA(path.c_str(), 0);
	return result == TRUE;
#else
	mode_t m = 0;
	m |= (user_permission << 6);
	m |= (group_permission << 3);
	m |= (other_permission << 0);
	int result = mkdir(path.c_str(), m);
	return (result == 0);
#endif
}

bool is_directory(const std::string &path) {
#ifdef WIN32
	struct _stat buf;
	int result = _stat(path.c_str(), &buf);
	if (result == 0 && S_ISDIR(buf.st_mode))
		return true;
	else
		return false;
#else
	struct stat buf;
	int result = stat(path.c_str(), &buf);
	if (result == 0 && S_ISDIR(buf.st_mode))
		return true;
	else
		return false;
#endif
}

bool list_directory(const std::string &directory, std::vector<std::string> &elements) {
#ifdef WIN32
	WIN32_FIND_DATA findData;
	HANDLE hFind = FindFirstFileA((directory + "\\*.*").c_str(), &findData);
	if (hFind == INVALID_HANDLE_VALUE) throw std::runtime_error("Failed to get directory list");
	for (;;) {
		if (findData.cFileName[0] != '.') {
			elements.push_back(findData.cFileName);
		}
		if (FindNextFileA(hFind, &findData) == 0) break;
	}
	FindClose(hFind);
#else
	DIR *dir = opendir(directory.c_str());
	if (dir == NULL) return false;
	dirent *entry;
	while ((entry = readdir(dir)) != NULL) {
		if (entry->d_name[0] != '.') {
			elements.push_back(entry->d_name);
		}
	}
	closedir(dir);
#endif
	return true;
}

bool create_directory_recursive(const std::string &dir, size_t user_permission, size_t group_permission,
                                size_t other_permission) {
	// split path
	const char seperators[] = "\\/";
	std::vector<std::string> elements;
	std::string::size_type a, b;
	a = dir.find_first_not_of(seperators), b = dir.find_first_of(seperators, a);
	while (a != std::string::npos) {
		elements.push_back(dir.substr(a, b - a));
		a = dir.find_first_not_of(seperators, b), b = dir.find_first_of(seperators, a);
	}

	// create all non existing parts
	std::string path;
	for (size_t i = 0; i < elements.size(); i++) {
		path += elements[i];
		path += path_seperator;
		if (is_directory(path) == false) create_directory(path);
	}

	return is_directory(dir);
}

std::string concat_path(const std::string &a, const std::string &b) {
	char last = *a.rbegin();
	if (last == '\\' || last == '/')
		return a + b;
	else
		return a + path_seperator + b;
}

std::string concat_path(const std::string &a, const std::string &b, const std::string &c) {
	std::string p = a;
	if (*p.rbegin() != '\\' && *p.rbegin() != '/') p += path_seperator;
	p += b;
	if (*p.rbegin() != '\\' && *p.rbegin() != '/') p += path_seperator;
	p += c;
	return p;
}

void append_file(const std::string &target, const std::string &source, bool binary) {
	FILE *t = fopen(target.c_str(), binary ? "wb+" : "w+");
	if (t == NULL) return;

	FILE *s = fopen(source.c_str(), binary ? "rb" : "r");
	if (s == NULL) {
		fclose(t);
		return;
	}

	size_t size = 0;
	char buffer[1 << 20];
	while ((size = fread(buffer, 1, sizeof(buffer), s)) > 0) {
		fwrite(buffer, 1, size, t);
	}

	fclose(t);
	fclose(s);
}

std::string executable_path() {
	char buf[1024];
	std::size_t len = 0;

#ifdef WIN32
	const DWORD result = ::GetModuleFileNameA(NULL, buf, sizeof(buf));
	if (result == 0 || result >= sizeof(buf)) return {};
	len = result;
#elif defined(__APPLE__)
	uint32_t bufferSize = sizeof(buf);
	if (_NSGetExecutablePath(buf, &bufferSize) != 0) return {};
	len = std::char_traits<char>::length(buf);
#else
	const ssize_t result = readlink("/proc/self/exe", buf, sizeof(buf));
	if (result <= 0 || static_cast<std::size_t>(result) >= sizeof(buf)) return {};
	len = static_cast<std::size_t>(result);
#endif

	const std::string path(buf, len);
	const std::size_t separator = path.find_last_of(path_seperator);
	if (separator == std::string::npos) return {};
	return path.substr(0, separator + 1);
}

std::string library_path() {
	std::string path;

#ifdef WIN32
	HMODULE module = NULL;
	const DWORD flags = GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT;
	if (!::GetModuleHandleExA(flags, reinterpret_cast<LPCSTR>(&library_path), &module)) return {};
	char buf[1024];
	const DWORD result = ::GetModuleFileNameA(module, buf, sizeof(buf));
	if (result == 0 || result >= sizeof(buf)) return {};
	path.assign(buf, result);
#else
	Dl_info info;
	if (dladdr(reinterpret_cast<const void *>(&library_path), &info) == 0 || info.dli_fname == nullptr) return {};
	path = info.dli_fname;
#endif

	const std::size_t separator = path.find_last_of("\\/");
	if (separator == std::string::npos) return {};
	return path.substr(0, separator + 1);
}
