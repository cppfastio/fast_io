#pragma once

/*
W3C standard
File System Access
Draft Community Group Report
https://wicg.github.io/file-system-access/
*/

namespace fast_io
{

enum class well_known_directory : ::std::uint_least32_t
{
	desktop,
	documents,
	downloads,
	music,
	pictures,
	videos
};

} // namespace fast_io
