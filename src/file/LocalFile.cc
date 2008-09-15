// $Id$

#include "probed_defs.hh"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef	HAVE_SYS_MMAN_H
#include <sys/mman.h>
#endif
#include "LocalFile.hh"
#include "FileOperations.hh"
#include "FileException.hh"
#include "PreCacheFile.hh"
#include <cstring> // for strchr
#include <cassert>

using std::string;

namespace openmsx {

LocalFile::LocalFile(const string& filename_, File::OpenMode mode)
	: filename(FileOperations::expandTilde(filename_))
	, readOnly(false)
{
	PRT_DEBUG("LocalFile: " << filename);

	if (mode == File::SAVE_PERSISTENT) {
		string::size_type pos = filename.find_last_of('/');
		if (pos != string::npos) {
			FileOperations::mkdirp(filename.substr(0, pos));
		}
	}

	const string name = FileOperations::getNativePath(filename);
	if ((mode == File::SAVE_PERSISTENT) || (mode == File::TRUNCATE)) {
		// open file read/write truncated
		file = fopen(name.c_str(), "wb+");
	} else if (mode == File::CREATE) {
		// open file read/write
		file = fopen(name.c_str(), "rb+");
		if (!file) {
			// create if it didn't exist yet
			file = fopen(name.c_str(), "wb+");
		}
	} else {
		// open file read/write
		file = fopen(name.c_str(), "rb+");
		if (!file) {
			// if that fails try read only
			file = fopen(name.c_str(), "rb");
			readOnly = true;
		}
	}
	if (!file) {
		throw FileException("Error opening file \"" + filename + "\"");
	}

	if (mode == File::PRE_CACHE) {
		cache.reset(new PreCacheFile(name));
	}
}

LocalFile::LocalFile(const std::string& filename, const char* mode)
{
	assert(strchr(mode, 'b'));
	const string name = FileOperations::getNativePath(filename);
	file = fopen(name.c_str(), mode);
	if (!file) {
		throw FileException("Error opening file \"" + filename + "\"");
	}
}

LocalFile::~LocalFile()
{
	munmap();
	fclose(file);
}

void LocalFile::read(void* buffer, unsigned num)
{
	long pos = ftell(file);
	fseek(file, 0, SEEK_END);
	unsigned size = unsigned(ftell(file));
	fseek(file, pos, SEEK_SET);
	if ((pos + num) > size) {
		throw FileException("Read beyond end of file");
	}

	fread(buffer, 1, num, file);
	if (ferror(file)) {
		throw FileException("Error reading file");
	}
}

void LocalFile::write(const void* buffer, unsigned num)
{
	fwrite(buffer, 1, num, file);
	if (ferror(file)) {
		throw FileException("Error writing file");
	}
}

#ifdef HAVE_MMAP
byte* LocalFile::mmap(bool writeBack)
{
	if (!mmem) {
		int flags = writeBack ? MAP_SHARED : MAP_PRIVATE;
		mmem = static_cast<byte*>(
		          ::mmap(0, getSize(), PROT_READ | PROT_WRITE,
		                 flags, fileno(file), 0));
		// MAP_FAILED is #define'd using an old-style cast, we
		// have to redefine it ourselves to avoid a warning
		void* MY_MAP_FAILED = reinterpret_cast<void*>(-1);
		if (mmem == MY_MAP_FAILED) {
			throw FileException("Error mmapping file");
		}
	}
	return mmem;
}

void LocalFile::munmap()
{
	if (mmem) {
		::munmap(mmem, getSize());
		mmem = NULL;
	}
}
#endif

unsigned LocalFile::getSize()
{
	long pos = ftell(file);
	fseek(file, 0, SEEK_END);
	unsigned size = unsigned(ftell(file));
	fseek(file, pos, SEEK_SET);
	return size;
}

void LocalFile::seek(unsigned pos)
{
	fseek(file, pos, SEEK_SET);
	if (ferror(file)) {
		throw FileException("Error seeking file");
	}
}

unsigned LocalFile::getPos()
{
	return unsigned(ftell(file));
}

#ifdef HAVE_FTRUNCATE
void LocalFile::truncate(unsigned size)
{
	int fd = fileno(file);
	if (ftruncate(fd, size)) {
		throw FileException("Error truncating file");
	}
}
#endif

void LocalFile::flush()
{
	fflush(file);
}

const string LocalFile::getURL() const
{
	return filename;
}

const string LocalFile::getLocalReference()
{
	return filename;
}

bool LocalFile::isReadOnly() const
{
	return readOnly;
}

time_t LocalFile::getModificationDate()
{
	struct stat st;
	if (fstat(fileno(file), &st)) {
		throw FileException("Cannot stat file");
	}
	return st.st_mtime;
}

} // namespace openmsx
