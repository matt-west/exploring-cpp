#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <locale>
#include <ostream>
#include <string>
#include <vector>

/// Read lines of text from @p in to @p iter. Lines are appended to @p iter.
/// @param in the input stream
/// @param iter an output iterator
template<class Ch, class Tr, class OutIter>
void read(std::basic_istream<Ch,Tr>& in, OutIter iter)
{
	std::basic_string<Ch,Tr> line;
	while (std::getline(in, line))
	{
		*iter = line;
		++iter;
	}
}

/// Sorter function object.
/// Parameterize the class with the character type used for strings.
/// The sorter object caches a locale and its @c collate facet, to use for
/// comparing styles.
template<typename Ch>
class sorter
{
public:
	/// Construct the sorter object, caching the locale that has the given name.
	/// @param locname The name of the locale to cache.
	sorter(Ch const* locname) :
		loc_(std::locale(locname)),
		collate_(std::use_facet<std::collate<Ch> >(loc_))
	{}

	/// Construct a default sorter object, using the global cache.
	sorter() :
		loc_(std::locale()),
		collate_(std::use_facet<std::collate<Ch> >(loc_))
	{}

	/// Compare for less-than, for use as a comparison predicate with any
	/// standard algorithm.
	/// @param lhs left-hand side operand
	/// @param rhs right-hand side operand
	/// return true if @p lhs < @p rhs in the given locale, false otherwise
	template<typename Tr>
	bool operator()(const std::basic_string<Ch,Tr>& lhs,
									const std::basic_string<Ch,Tr>& rhs)
				{
					return collate_.compare(lhs.data(), lhs.data()+lhs.size(),
																	rhs.data(), rhs.data()+rhs.size()) < 0;
				}
private:
	std::locale loc_;
	const std::collate<Ch>& collate_;
};

/// Make a sorter object by deducing the template parameter.
/// @param name the locale name to pass to the sorter constructor
/// @return a new sorter object
template<typename Ch>
sorter<Ch> make_sorter(Ch const * name)
{
	return sorter<Ch>(name);
}

/// Main Program.
int main(int argc, char* argv[])
try
{
	// Throw an exception if an unrecoverable input error occurs, e.g.,
	// disk failure.
	std::cin.exceptions(std::ios_base::badbit);

	// Read the entire input to text. If command line specifies a file, read that.
	// Otherwise, read the standard input.
	std::vector<std::string> text;
	if (argc < 2)
		read(std::cin, std::back_inserter(text));
	else
	{
		std::ifstream in(argv[1]);
		if (not in)
		{
			std::perror(argv[1]);	return EXIT_FAILURE;
		}
		read(in, std::back_inserter(text));
	}

	// Sort the text. The second command line argument, if present,
	// names a locale, to control the sort order. Without a command line
	// argument, use the default locale (which is obtained from the OS).
	std::sort(text.begin(), text.end(), make_sorter(argc >= 3 ? argv[2] : ""));

	// Print the sorted text.
	std::copy(text.begin(), text.end(),
						std::ostream_iterator<std::string>(std::cout, "\n"));
}
catch (std::exception& ex)
{
	std::cerr << "Caught exception: " << ex.what() << '\n';
	std::cerr << "Terminating program.\n";
	std::exit(EXIT_FAILURE);
}
catch (...)
{
	std::cerr << "Caught unknown exception type.\nTerminating program.\n";
	std::exit(EXIT_FAILURE);
}