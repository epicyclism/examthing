// Derived from Boost Beast examples copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
#include <optional>
#include <charconv>
#include <array>
//#include <fmt/format.h>

#include "http_common.h"

inline char ascii_tolower(char c)
{
    return ((static_cast<unsigned>(c) - 65U) < 26) ?
        c + 'a' - 'A' : c;
}

bool
iequals(
    std::string_view lhs,
    std::string_view rhs)
{
    auto n = lhs.size();
    if (rhs.size() != n)
        return false;
    auto p1 = lhs.data();
    auto p2 = rhs.data();
    char a, b;
    // fast loop
    while (n--)
    {
        a = *p1++;
        b = *p2++;
        if (a != b)
            goto slow;
    }
    return true;
slow:
    do
    {
        if (ascii_tolower(a) !=
            ascii_tolower(b))
            return false;
        a = *p1++;
        b = *p2++;
    } while (n--);
    return true;
}

// Return a reasonable mime type based on the extension of a file.
std::string_view
mime_type(std::string_view path)
{
    auto const ext = [&path]
    {
        auto const pos = path.rfind(".");
        if (pos == std::string_view::npos)
            return std::string_view{};
        return path.substr(pos);
    }();
    if (iequals(ext, ".htm"))  return "text/html";
    if (iequals(ext, ".html")) return "text/html";
    if (iequals(ext, ".mp4"))  return "video/mpeg";
    if (iequals(ext, ".ico"))  return "image/vnd.microsoft.icon";
    if (iequals(ext, ".css"))  return "text/css";
    if (iequals(ext, ".png"))  return "image/png";
    if (iequals(ext, ".php"))  return "text/html";
    if (iequals(ext, ".txt"))  return "text/plain";
    if (iequals(ext, ".js"))   return "application/javascript";
    if (iequals(ext, ".json")) return "application/json";
    if (iequals(ext, ".xml"))  return "application/xml";
    if (iequals(ext, ".swf"))  return "application/x-shockwave-flash";
    if (iequals(ext, ".flv"))  return "video/x-flv";
    if (iequals(ext, ".jpe"))  return "image/jpeg";
    if (iequals(ext, ".jpeg")) return "image/jpeg";
    if (iequals(ext, ".jpg"))  return "image/jpeg";
    if (iequals(ext, ".gif"))  return "image/gif";
    if (iequals(ext, ".bmp"))  return "image/bmp";
    if (iequals(ext, ".tiff")) return "image/tiff";
    if (iequals(ext, ".tif"))  return "image/tiff";
    if (iequals(ext, ".svg"))  return "image/svg+xml";
    if (iequals(ext, ".svgz")) return "image/svg+xml";
    return "application/text";
}

// Append an HTTP rel-path to a local filesystem path.
// The returned path is normalized for the platform.
std::string
path_cat(
    std::string_view base,
    std::string_view path)
{
    if (base.empty())
        return std::string(path);
    std::string result(base);
    auto ps = path.find_first_of("?");
    if (ps != std::string_view::npos)
        path.remove_suffix(path.size() - ps);
#ifdef BOOST_MSVC
    char constexpr path_separator = '\\';
    if (result.back() == path_separator)
        result.resize(result.size() - 1);
    result.append(path.data(), path.size());
    for (auto& c : result)
        if (c == '/')
            c = path_separator;
#else
    char constexpr path_separator = '/';
    if (result.back() == path_separator)
        result.resize(result.size() - 1);
    result.append(path.data(), path.size());
#endif
    return result;
}

int is_hex(char c)
{
    switch (c)
    {
    case '0': return 0;
    case '1': return 1;
    case '2': return 2;
    case '3': return 3; 
    case '4': return 4;
    case '5': return 5;
    case '6': return 6; 
    case '7': return 7;
    case '8': return 8;
    case '9': return 9;
    case 'a': return 10;
    case 'b': return 11;
    case 'c': return 12;
    case 'd': return 13;
    case 'e': return 14;
    case 'f': return 15;
    case 'A': return 10;
    case 'B': return 11;
    case 'C': return 12;
    case 'D': return 13;
    case 'E': return 14;
    case 'F': return 15;
    default:
        break;
    }
    return -1;
}

std::optional<char> to_ascii(std::string const& s, std::string::size_type p)
{
    auto h = is_hex(s[p]);
    auto l = is_hex(s[p + 1]);
    if (h == -1 || l == -1)
        return std::nullopt;
    return h * 16 + l;
}

// do this in place
void percent_decode(std::string& s)
{
    // fix this up to something nice sometime
    auto p = s.find('%');
    while (p != std::string::npos)
    {
        if (auto nc = to_ascii(s, p + 1); nc)
        {
            // us this the worst inteface ever?
            s.replace(p, 3, 1, nc.value());
        }
        p = s.find('%', p + 1);
    }
}
