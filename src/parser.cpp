#include "fire/parser.hpp"

template <>
bool ConfigOption<bool>::parse(std::vector<std::string>& args)
/* We assume if this option is set, then it must be true */
{
    if (args.empty()) return true;
    for (auto iter = args.begin(); iter < args.end(); ++iter)
    {
        if (auto pos = check_short_name_option(*iter); *iter == "--" + long_name || pos)
        {
            _data = true;
            if (pos && iter->length() > 1) break;
            args.erase(iter, iter + 1);
            break;
        }
    }
    return false;
}