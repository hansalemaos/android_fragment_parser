#include <algorithm>
#include <array>
#include <cstring>
#include <fstream>
#include <iostream>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>
#if __has_include(<format>)
#include <format>
#define HAS_STD_FORMAT 1
#else
#define HAS_STD_FORMAT 0
#endif
#ifdef _WIN32
#include <windows.h>
#define EXEC_CMD(command, mode) _popen(command, mode)
#define CLOSE_CMD(pipe) _pclose(pipe)
#else
#include <unistd.h>
#define EXEC_CMD(command, mode) popen(command, mode)
#define CLOSE_CMD(pipe) pclose(pipe)
#endif

typedef struct fragment_data
{
    std::string ORIGINAL_STRING;
    std::string CLASSNAME;
    std::string ELEMENT_ID;
    std::string HASHCODE;
    std::string MID;
    std::string MY_PARENT_IDS;
    int64_t HASHCODE_INT;
    int64_t MID_INT;
    int MY_ID;
    int MY_GROUP_ID;
    int MY_ELEMENT_ID;
    int MY_DIRECT_PARENT_ID;
    int SPACES;
    int AREA;
    int CENTER_X;
    int CENTER_Y;
    int END_X;
    int END_X_RELATIVE;
    int END_Y;
    int END_Y_RELATIVE;
    int HEIGHT;
    int START_X;
    int START_X_RELATIVE;
    int START_Y;
    int START_Y_RELATIVE;
    int WIDTH;
    int IS_SQARE;
    float REL_WIDTH_HEIGHT;
    char CLICKABLE;
    char CONTEXT_CLICKABLE;
    char DRAWN;
    char ENABLED;
    char FOCUSABLE;
    char LONG_CLICKABLE;
    char PFLAG_ACTIVATED;
    char PFLAG_DIRTY_MASK;
    char PFLAG_FOCUSED;
    char PFLAG_HOVERED;
    char PFLAG_INVALIDATED;
    char PFLAG_IS_ROOT_NAMESPACE;
    char PFLAG_PREPRESSED;
    char PFLAG_SELECTED;
    char SCROLLBARS_HORIZONTAL;
    char SCROLLBARS_VERTICAL;
    char VISIBILITY;

} fr_data;
static constexpr int count_leading_spaces(const std::string_view line)
{
    int count = 0;
    for (char c : line)
    {
        if (c == ' ')
        {
            count++;
        }
        else
        {
            break;
        }
    }
    return (int)count;
}
static constexpr int find_start_index_of_substring(const std::string_view haystack, const std::string_view needle)
{
    size_t start_index = haystack.find(needle);
    if (start_index == std::string::npos)
    {
        return -1;
    }
    return start_index;
}

static std::array<int, 4> get_coords(std::string_view &s)
{
    std::array<int, 4> v{0, 0, 0, 0};
    int idx_number_1{find_start_index_of_substring(s, ",")};
    if (idx_number_1 == -1)
    {
        return v;
    }
    std::string_view number_1{s.substr(0, idx_number_1)};
    s = s.substr(idx_number_1 + 1);
    int idx_number_2{find_start_index_of_substring(s, "-")};
    if (idx_number_2 == -1)
    {
        return v;
    }
    std::string_view number_2{s.substr(0, idx_number_2)};
    s = s.substr(idx_number_2 + 1);
    int idx_number_3{find_start_index_of_substring(s, ",")};
    if (idx_number_3 == -1)
    {
        return v;
    }
    std::string_view number_3{s.substr(0, idx_number_3)};
    s = s.substr(idx_number_3 + 1);

    int idx_number_4{find_start_index_of_substring(s, " ")};
    if (idx_number_4 == -1)
    {
        idx_number_4 = find_start_index_of_substring(s, "}");
        if (idx_number_4 == -1)
        {
            return v;
        }
    }
    std::string_view number_4{s.substr(0, idx_number_4)};
    s = s.substr(idx_number_4 + 1);
    v[0] = std::stoi(number_1.data());
    v[1] = std::stoi(number_2.data());
    v[2] = std::stoi(number_3.data());
    v[3] = std::stoi(number_4.data());
    return v;
}

static std::vector<fragment_data> parse_raw_elements(std::string_view str)
{
    auto strs = str | std::views::split('\n');
    size_t sizecounter{0};
    for (const auto &ref : strs)
    {
        sizecounter++;
    }
    std::vector<fragment_data> resultvector;
    resultvector.reserve(sizecounter);
    for (const auto &ref : strs)
    {
        std::string rstringoriginal{ref.begin(), ref.end()};
        std::string_view r{rstringoriginal};
        int r_leading_spaces{count_leading_spaces(r)};
        std::string_view r_without_leading_spaces{r.substr(r_leading_spaces)};
        resultvector.emplace_back(fragment_data{});
        fragment_data &dataline = resultvector.back();
        dataline.SPACES = r_leading_spaces;
        dataline.ORIGINAL_STRING = rstringoriginal;
        ////////////////////////////////////////////////////////////////////////////////
        // CLASSNAME
        int end_str_classname = find_start_index_of_substring(r_without_leading_spaces, "{");
        if (end_str_classname != -1)
        {
            dataline.CLASSNAME = r_without_leading_spaces.substr(0, end_str_classname);
            r_without_leading_spaces = r_without_leading_spaces.substr(end_str_classname + 1);
        }
        ////////////////////////////////////////////////////////////////////////////////
        // HASHCODE
        int end_str_hashcode = find_start_index_of_substring(r_without_leading_spaces, " ");
        if (end_str_hashcode != -1)
        {
            dataline.HASHCODE = r_without_leading_spaces.substr(0, end_str_hashcode);
            dataline.HASHCODE_INT = std::stoi(dataline.HASHCODE, nullptr, 16);
            r_without_leading_spaces = r_without_leading_spaces.substr(end_str_hashcode + 1);
        }

        if (r_without_leading_spaces.size() < 20)
        {
            continue;
        }
        dataline.VISIBILITY = r_without_leading_spaces[0];
        dataline.FOCUSABLE = r_without_leading_spaces[1];
        dataline.ENABLED = r_without_leading_spaces[2];
        dataline.DRAWN = r_without_leading_spaces[3];
        dataline.SCROLLBARS_HORIZONTAL = r_without_leading_spaces[4];
        dataline.SCROLLBARS_VERTICAL = r_without_leading_spaces[5];
        dataline.CLICKABLE = r_without_leading_spaces[6];
        dataline.LONG_CLICKABLE = r_without_leading_spaces[7];
        dataline.CONTEXT_CLICKABLE = r_without_leading_spaces[8];
        dataline.PFLAG_IS_ROOT_NAMESPACE = r_without_leading_spaces[10];
        dataline.PFLAG_FOCUSED = r_without_leading_spaces[11];
        dataline.PFLAG_SELECTED = r_without_leading_spaces[12];
        dataline.PFLAG_PREPRESSED = r_without_leading_spaces[13];
        dataline.PFLAG_HOVERED = r_without_leading_spaces[14];
        dataline.PFLAG_ACTIVATED = r_without_leading_spaces[15];
        dataline.PFLAG_INVALIDATED = r_without_leading_spaces[16];
        dataline.PFLAG_DIRTY_MASK = r_without_leading_spaces[17];
        r_without_leading_spaces = r_without_leading_spaces.substr(19);
        auto dataline_INT_COORDS = get_coords(r_without_leading_spaces);
        dataline.START_X_RELATIVE = dataline_INT_COORDS[0];
        dataline.START_Y_RELATIVE = dataline_INT_COORDS[1];
        dataline.END_X_RELATIVE = dataline_INT_COORDS[2];
        dataline.END_Y_RELATIVE = dataline_INT_COORDS[3];

        dataline.START_X = dataline_INT_COORDS[0];
        dataline.START_Y = dataline_INT_COORDS[1];
        dataline.END_X = dataline_INT_COORDS[2];
        dataline.END_Y = dataline_INT_COORDS[3];

        dataline.WIDTH = dataline.END_X_RELATIVE - dataline.START_X_RELATIVE;
        dataline.HEIGHT = dataline.END_Y_RELATIVE - dataline.START_Y_RELATIVE;
        dataline.AREA = dataline.WIDTH * dataline.HEIGHT;

        if (r_without_leading_spaces.size() < 2)
        {
            continue;
        }
        if (r_without_leading_spaces[0] == '#')
        {
            ////////////////////////////////////////////////////////////////////////////////
            // MID
            int end_str_mid = find_start_index_of_substring(r_without_leading_spaces, " ");
            if (end_str_mid != -1)
            {
                dataline.MID = r_without_leading_spaces.substr(1, end_str_mid);
                dataline.MID_INT = std::stoi(dataline.MID, nullptr, 16);
                r_without_leading_spaces = r_without_leading_spaces.substr(end_str_mid + 1);
            }
        }
        ////////////////////////////////////////////////////////////////////////////////
        // ELEMENT_ID
        int end_str_elementid = find_start_index_of_substring(r_without_leading_spaces, "}");
        if (end_str_elementid != -1)
        {
            dataline.ELEMENT_ID = r_without_leading_spaces.substr(0, end_str_elementid);
        }
    }
    auto it{resultvector.begin()};
    int myidcounter{0};
    while (it != resultvector.end())
    {
        if (it->SPACES == 0)
        {
            it = resultvector.erase(it);
        }
        else
        {
            it->MY_ID = myidcounter;
            myidcounter++;
            it++;
        }
    }
    return resultvector;
}

static constexpr const char *csv_header =
    "\"MY_ID\",\"MY_GROUP_ID\",\"MY_ELEMENT_ID\",\"MY_DIRECT_PARENT_ID\",\"MY_PARENT_IDS\",\"ORIGINAL_STRING\","
    "\"CENTER_X\",\"CENTER_Y\",\"AREA\",\"START_X\",\"START_Y\",\"END_X\",\"END_Y\",\"HEIGHT\",\"WIDTH\",\"IS_SQARE\","
    "\"REL_WIDTH_HEIGHT\",\"HASHCODE_INT\",\"MID_INT\",\"SPACES\",\"CLASSNAME\",\"ELEMENT_ID\",\"HASHCODE\",\"MID\","
    "\"START_X_RELATIVE\",\"END_X_RELATIVE\",\"START_Y_RELATIVE\",\"END_Y_RELATIVE\",\"CLICKABLE\",\"CONTEXT_"
    "CLICKABLE\",\"DRAWN\",\"ENABLED\",\"FOCUSABLE\",\"LONG_CLICKABLE\",\"PFLAG_ACTIVATED\",\"PFLAG_DIRTY_MASK\","
    "\"PFLAG_FOCUSED\",\"PFLAG_HOVERED\",\"PFLAG_INVALIDATED\",\"PFLAG_IS_ROOT_NAMESPACE\",\"PFLAG_PREPRESSED\","
    "\"PFLAG_SELECTED\",\"SCROLLBARS_HORIZONTAL\",\"SCROLLBARS_VERTICAL\",\"VISIBILITY\"";

static constexpr std::string_view delim_csv{"\",\""};

static std::string dump_struct_vector_as_csv(std::vector<fragment_data> &v)
{
    std::string outputstring;
    if (!v.empty())
    {
        size_t maxsize{sizeof(v[0])};
        for (const auto &ref : v)
        {
            if (sizeof(ref) > maxsize)
            {
                maxsize = sizeof(ref);
            }
        }
        outputstring.reserve(v.size() * maxsize * 2);
        outputstring.append(csv_header);
        outputstring += '\n';
    }
    else
    {
        return outputstring;
    }

    std::vector<fragment_data>::iterator it{v.begin()};
    while (it != v.end())
    {
#if HAS_STD_FORMAT
        outputstring.append(std::format(
            "\"{}\",\"{}\",\"{}\",\"{}\",\"{}\",\"{}\",\"{}\",\"{}\",\"{}\",\"{}\",\"{}\",\"{}\",\"{}\",\"{}\",\"{}\","
            "\"{}\",\"{}\",\"{}\",\"{}\",\"{}\",\"{}\",\"{}\",\"{}\",\"{}\",\"{}\",\"{}\",\"{}\",\"{}\",\"{}\",\"{}\","
            "\"{}\",\"{}\",\"{}\",\"{}\",\"{}\",\"{}\",\"{}\",\"{}\",\"{}\",\"{}\",\"{}\",\"{}\",\"{}\",\"{}\",\"{}"
            "\"\n",
            it->MY_ID, it->MY_GROUP_ID, it->MY_ELEMENT_ID, it->MY_DIRECT_PARENT_ID, it->MY_PARENT_IDS,
            it->ORIGINAL_STRING, it->CENTER_X, it->CENTER_Y, it->AREA, it->START_X, it->START_Y, it->END_X, it->END_Y,
            it->HEIGHT, it->WIDTH, it->IS_SQARE, it->REL_WIDTH_HEIGHT, it->HASHCODE_INT, it->MID_INT, it->SPACES,
            it->CLASSNAME, it->ELEMENT_ID, it->HASHCODE, it->MID, it->START_X_RELATIVE, it->END_X_RELATIVE,
            it->START_Y_RELATIVE, it->END_Y_RELATIVE, it->CLICKABLE, it->CONTEXT_CLICKABLE, it->DRAWN, it->ENABLED,
            it->FOCUSABLE, it->LONG_CLICKABLE, it->PFLAG_ACTIVATED, it->PFLAG_DIRTY_MASK, it->PFLAG_FOCUSED,
            it->PFLAG_HOVERED, it->PFLAG_INVALIDATED, it->PFLAG_IS_ROOT_NAMESPACE, it->PFLAG_PREPRESSED,
            it->PFLAG_SELECTED, it->SCROLLBARS_HORIZONTAL, it->SCROLLBARS_VERTICAL, it->VISIBILITY));
#else
        outputstring += '"';
        outputstring.append(std::to_string(it->MY_ID));
        outputstring.append(delim_csv);
        outputstring.append(std::to_string(it->MY_GROUP_ID));
        outputstring.append(delim_csv);
        outputstring.append(std::to_string(it->MY_ELEMENT_ID));
        outputstring.append(delim_csv);
        outputstring.append(std::to_string(it->MY_DIRECT_PARENT_ID));
        outputstring.append(delim_csv);
        outputstring.append(it->MY_PARENT_IDS);
        outputstring.append(delim_csv);
        outputstring.append(it->ORIGINAL_STRING);
        outputstring.append(delim_csv);
        outputstring.append(std::to_string(it->CENTER_X));
        outputstring.append(delim_csv);
        outputstring.append(std::to_string(it->CENTER_Y));
        outputstring.append(delim_csv);
        outputstring.append(std::to_string(it->AREA));
        outputstring.append(delim_csv);
        outputstring.append(std::to_string(it->START_X));
        outputstring.append(delim_csv);
        outputstring.append(std::to_string(it->START_Y));
        outputstring.append(delim_csv);
        outputstring.append(std::to_string(it->END_X));
        outputstring.append(delim_csv);
        outputstring.append(std::to_string(it->END_Y));
        outputstring.append(delim_csv);
        outputstring.append(std::to_string(it->HEIGHT));
        outputstring.append(delim_csv);
        outputstring.append(std::to_string(it->WIDTH));
        outputstring.append(delim_csv);
        outputstring.append(std::to_string(it->IS_SQARE));
        outputstring.append(delim_csv);
        outputstring.append(std::to_string(it->REL_WIDTH_HEIGHT));
        outputstring.append(delim_csv);
        outputstring.append(std::to_string(it->HASHCODE_INT));
        outputstring.append(delim_csv);
        outputstring.append(std::to_string(it->MID_INT));
        outputstring.append(delim_csv);
        outputstring.append(std::to_string(it->SPACES));
        outputstring.append(delim_csv);
        outputstring.append(it->CLASSNAME);
        outputstring.append(delim_csv);
        outputstring.append(it->ELEMENT_ID);
        outputstring.append(delim_csv);
        outputstring.append(it->HASHCODE);
        outputstring.append(delim_csv);
        outputstring.append(it->MID);
        outputstring.append(delim_csv);
        outputstring.append(std::to_string(it->START_X_RELATIVE));
        outputstring.append(delim_csv);
        outputstring.append(std::to_string(it->END_X_RELATIVE));
        outputstring.append(delim_csv);
        outputstring.append(std::to_string(it->START_Y_RELATIVE));
        outputstring.append(delim_csv);
        outputstring.append(std::to_string(it->END_Y_RELATIVE));
        outputstring.append(delim_csv);
        outputstring += it->CLICKABLE;
        outputstring.append(delim_csv);
        outputstring += it->CONTEXT_CLICKABLE;
        outputstring.append(delim_csv);
        outputstring += it->DRAWN;
        outputstring.append(delim_csv);
        outputstring += it->ENABLED;
        outputstring.append(delim_csv);
        outputstring += it->FOCUSABLE;
        outputstring.append(delim_csv);
        outputstring += it->LONG_CLICKABLE;
        outputstring.append(delim_csv);
        outputstring += it->PFLAG_ACTIVATED;
        outputstring.append(delim_csv);
        outputstring += it->PFLAG_DIRTY_MASK;
        outputstring.append(delim_csv);
        outputstring += it->PFLAG_FOCUSED;
        outputstring.append(delim_csv);
        outputstring += it->PFLAG_HOVERED;
        outputstring.append(delim_csv);
        outputstring += it->PFLAG_INVALIDATED;
        outputstring.append(delim_csv);
        outputstring += it->PFLAG_IS_ROOT_NAMESPACE;
        outputstring.append(delim_csv);
        outputstring += it->PFLAG_PREPRESSED;
        outputstring.append(delim_csv);
        outputstring += it->PFLAG_SELECTED;
        outputstring.append(delim_csv);
        outputstring += it->SCROLLBARS_HORIZONTAL;
        outputstring.append(delim_csv);
        outputstring += it->SCROLLBARS_VERTICAL;
        outputstring.append(delim_csv);
        outputstring += it->VISIBILITY;
        outputstring += '"';
        outputstring += '\n';

#endif
        it++;
    }
    return outputstring;
}
static std::string fragment_dump_to_csv(std::string_view str)
{
    std::vector<fragment_data> resultvector{parse_raw_elements(str)};
    if (resultvector.empty())
    {
        return "";
    }
    std::vector<std::vector<fragment_data>> grouped_elements;
    grouped_elements.reserve(resultvector.size());
    for (int i{(int)resultvector.size() - 1}; i >= 0; i--)
    {
        int spaces_of_element = resultvector[i].SPACES;
        int spaces_of_current_parent = resultvector[i].SPACES;
        grouped_elements.emplace_back(std::vector<fragment_data>{resultvector[i]});
        for (int j = i; j >= 0; j--)
        {

            if (resultvector[j].SPACES < spaces_of_current_parent)
            {

                grouped_elements.back().emplace_back(resultvector[j]);
                spaces_of_current_parent = resultvector[j].SPACES;
            }
        }
        std::reverse(grouped_elements.back().begin(), grouped_elements.back().end());
    }
    int mygroupidcounter = -1;
    for (auto &gref : grouped_elements)
    {
        mygroupidcounter++;
        int myelementidcounter = 0;
        for (auto &ggref : gref)
        {
            ggref.MY_GROUP_ID = mygroupidcounter;
            ggref.MY_ELEMENT_ID = myelementidcounter++;
        }
    }
    int minspacescounter = 40960;
    int finalvector_size = 0;
    for (auto &gref : grouped_elements)
    {
        std::string allmyparentids;
        allmyparentids.reserve(gref.size() * 10);
        for (size_t i{}; i < gref.size() - 1; i++)
        {
            gref[i + 1].MY_DIRECT_PARENT_ID = gref[i].MY_ID;
            gref[i + 1].START_X += gref[i].START_X;
            gref[i + 1].START_Y += gref[i].START_Y;
            gref[i + 1].END_X += gref[i].START_X;
            gref[i + 1].END_Y += gref[i].START_Y;
            allmyparentids += std::to_string(gref[i].MY_ID) + ",";

            gref[i + 1].MY_PARENT_IDS += allmyparentids;

            if ((i == gref.size() - 2) && (!gref[i + 1].MY_PARENT_IDS.empty()))
            {
                gref[i + 1].MY_PARENT_IDS.pop_back();
            }
            if (gref[i].SPACES < minspacescounter)
            {
                minspacescounter = gref[i].SPACES;
            }
            finalvector_size++;
        }
    }
    std::vector<fragment_data> finalvector;
    finalvector.reserve(finalvector_size);
    for (auto &gref : grouped_elements)
    {
        for (size_t i{}; i < gref.size(); i++)
        {
            gref[i].ORIGINAL_STRING = gref[i].ORIGINAL_STRING.substr(minspacescounter);
            gref[i].CENTER_X = (gref[i].START_X + gref[i].END_X) / 2;
            gref[i].CENTER_Y = (gref[i].START_Y + gref[i].END_Y) / 2;
            gref[i].IS_SQARE = gref[i].WIDTH == gref[i].HEIGHT;
            if (gref[i].HEIGHT > 0)
            {
                gref[i].REL_WIDTH_HEIGHT = (float)gref[i].WIDTH / (float)gref[i].HEIGHT;
            }
            finalvector.emplace_back(gref[i]);
        }
    }
    return dump_struct_vector_as_csv(finalvector);
}
static constexpr std::string_view viewhierachystring{" View Hierarchy:"};
static constexpr std::string_view looperstring{" Looper "};

int main(int argc, char *argv[])
{
    std::string cmd;
    if (argc < 2)
    {
        cmd = "dumpsys activity top -a -c --checkin";
        /* zig c++ -std=c++2a -O3 -g0 fragmentdumper.cpp && a "adb -s 127.0.0.1:5556 shell dumpsys activity top -a -c
        --checkin" */

        /* g++ - 12 - fpermissive - std = c++2a - O2 - g0 fragmentdumper.cpp && ./a.out "adb -s 127.0.0.1:5565 shell
         * dumpsys activity top -a -c --checkin" */
    }
    else
    {
        cmd = argv[1];
    }
    FILE *pipe = NULL;
    static constexpr size_t size_my_buffer = 32;
    char buffer[size_my_buffer]{};
    pipe = EXEC_CMD(cmd.c_str(), "r");
    if (!pipe)
    {
        std::cerr << "ERROR: failed to execute command: " << cmd << std::endl;
        return 1;
    }
    std::string cpptmpstring;
    cpptmpstring.reserve(1024);
    std::vector<std::string> stringvector;
    size_t last_view_hierachy_index{};
    size_t last_looper_index{};
    while (fgets(buffer, size_my_buffer, pipe) != NULL)
    {
        for (size_t i{}; i < size_my_buffer; i++)
        {
            if (buffer[i] == '\0')
            {
                continue;
            }
            if (buffer[i] == '\n')
            {
                buffer[i] = '\0';
#ifdef _WIN32
                if (cpptmpstring.back() == '\r')
                {
                    cpptmpstring.pop_back();
                }
#endif
                stringvector.emplace_back(cpptmpstring);
                if (find_start_index_of_substring(cpptmpstring, viewhierachystring) != -1)
                {
                    last_view_hierachy_index = stringvector.size() - 1;
                }
                else if (find_start_index_of_substring(cpptmpstring, looperstring) != -1)
                {
                    last_looper_index = stringvector.size() - 1;
                }
                cpptmpstring.clear();
            }
            else
            {
                cpptmpstring += buffer[i];
                buffer[i] = '\0';
            }
        }
    }

    std::string str;
    str.reserve(40960);
    bool found_first_with_curly_braces_end{false};
    for (size_t i = last_view_hierachy_index + 0; i < last_looper_index; i++)
    {
        if ((!found_first_with_curly_braces_end))
        {
            if (find_start_index_of_substring(stringvector[i], "}") != -1)
            {
                found_first_with_curly_braces_end = true;
            }
            else
            {
                continue;
            }
        }

        str.append(stringvector[i]);
        str += "\n";
    }
    std::string csvresult{fragment_dump_to_csv(str)};
    std::cout << csvresult << std::endl;
    return 0;
    // Let the os do the cleanup
    // CLOSE_CMD(pipe);
}
