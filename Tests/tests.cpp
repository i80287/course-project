#include <cstdint>
#include <exception>
#include <functional>
#include <iostream>

#include "../App/ACTrie.hpp"
#include "../App/Observer.hpp"
#include "Timer.hpp"

namespace AppSpace {

using ACTrie = ACTrieDS::ACTrie;

struct TestImplResult final {
    bool passed;
    std::size_t expected_occurances_size;
    Timer::Duration time_passed;
};

struct TestResult final {
    bool passed;
    std::size_t found_occurances_size;
    std::size_t expected_occurances_size;
    std::size_t patterns_size;
    std::size_t text_size;
    Timer::Duration time_passed;
};

template <size_t PatternsSize>
TestImplResult RunTests(
    const std::string_view (&patterns)[PatternsSize],
    std::string_view text,
    const std::vector<std::pair<std::string_view, size_t>>&
        expected_occurances) {
    ACTrie actrie;
    for (std::string_view pattern : patterns) {
        actrie.AddPattern(pattern);
    }
    if (actrie.PatternsSize() != PatternsSize) {
        return {false, {}, {}};
    }

    std::vector<std::pair<std::string_view, size_t>> found_occurances_size;
    found_occurances_size.reserve(expected_occurances.size());
    ACTrie::FoundSubstringObserver found_substrings_obs(
        [&found_occurances_size](ACTrie::FoundSubstringInfoPassBy info) {
            found_occurances_size.emplace_back(info.found_substring,
                                               info.substring_start_index);
        });
    actrie.AddSubscriber(&found_substrings_obs);

    Timer timer;
    actrie.FindAllSubstringsInText(text);
    auto time_passed = timer.TimePassed();

    return {
        .passed = found_occurances_size == expected_occurances,
        .expected_occurances_size = found_occurances_size.size(),
        .time_passed              = time_passed,
    };
}

TestResult Test1Impl() {
    constexpr std::string_view patterns[] = {"a",  "ab", "ba",
                                             "aa", "bb", "fasb"};
    constexpr std::string_view text       = "ababcdacafaasbfasbabcc";
    const std::vector<std::pair<std::string_view, size_t>>
        expected_occurances = {
            {"a", 0},  {"ab", 0},    {"ba", 1},  {"a", 2},   {"ab", 2},
            {"a", 6},  {"a", 8},     {"a", 10},  {"aa", 10}, {"a", 11},
            {"a", 15}, {"fasb", 14}, {"ba", 17}, {"a", 18},  {"ab", 18},
        };
    const auto [passed, found_occurances_size, time_passed] =
        RunTests(patterns, text, expected_occurances);
    return {
        .passed                   = passed,
        .found_occurances_size    = found_occurances_size,
        .expected_occurances_size = expected_occurances.size(),
        .patterns_size            = std::size(patterns),
        .text_size                = text.size(),
        .time_passed              = time_passed,
    };
}

TestResult Test2Impl() {
    constexpr std::string_view patterns[] = {"ABC", "CDE", "CDEF"};
    constexpr std::string_view text =
        "ABCDEFGHABCDEFGADCVABCDEBACBCBABDBEBCBABABBCDEBCBABDEBCABDBCBACAB"
        "CDBEBACBCDEWBCBABCDE";
    const std::vector<std::pair<std::string_view, size_t>>
        expected_occurances = {
            {"ABC", 0},  {"CDE", 2},   {"CDEF", 2}, {"ABC", 8},
            {"CDE", 10}, {"CDEF", 10}, {"ABC", 19}, {"CDE", 21},
            {"CDE", 43}, {"ABC", 63},  {"CDE", 73}, {"ABC", 80},
            {"CDE", 82},
        };
    const auto [passed, found_occurances_size, time_passed] =
        RunTests(patterns, text, expected_occurances);
    return {
        .passed                   = passed,
        .found_occurances_size    = found_occurances_size,
        .expected_occurances_size = expected_occurances.size(),
        .patterns_size            = std::size(patterns),
        .text_size                = text.size(),
        .time_passed              = time_passed,
    };
}

TestResult Test3Impl() {
    constexpr std::string_view patterns[] = {"aba", "baca", "abacaba",
                                             "ccbba", "cabaaba"};
    constexpr std::string_view text =
        "ababcbbacbcabaabaacbacbbacbbabcbabcbcabaabaabcabaabacabaabacbabbb"
        "acbabacbabbacbcabacabcbcbacabaababcbabbacacbbcbcababbcbcbacabcaba"
        "cbcababacababcbabccaababacabcbabcbacbabcabcbbababacaababababcbbcb"
        "cbcbcbcbababcbabcabccbbcbcbcabaabacabbacbabca";
    const std::vector<std::pair<std::string_view, size_t>>
        expected_occurances = {
            {"aba", 0},       {"aba", 11},     {"cabaaba", 10},
            {"aba", 14},      {"aba", 37},     {"cabaaba", 36},
            {"aba", 40},      {"aba", 46},     {"cabaaba", 45},
            {"aba", 49},      {"baca", 50},    {"abacaba", 49},
            {"aba", 53},      {"cabaaba", 52}, {"aba", 56},
            {"aba", 68},      {"aba", 80},     {"baca", 81},
            {"baca", 89},     {"aba", 92},     {"cabaaba", 91},
            {"aba", 95},      {"baca", 103},   {"aba", 113},
            {"baca", 121},    {"aba", 127},    {"aba", 133},
            {"aba", 135},     {"baca", 136},   {"abacaba", 135},
            {"aba", 139},     {"aba", 150},    {"aba", 152},
            {"baca", 153},    {"aba", 175},    {"aba", 177},
            {"baca", 178},    {"aba", 182},    {"aba", 184},
            {"aba", 186},     {"aba", 203},    {"aba", 223},
            {"cabaaba", 222}, {"aba", 226},    {"baca", 227},
        };
    const auto [passed, found_occurances_size, time_passed] =
        RunTests(patterns, text, expected_occurances);
    return {
        .passed                   = passed,
        .found_occurances_size    = found_occurances_size,
        .expected_occurances_size = expected_occurances.size(),
        .patterns_size            = std::size(patterns),
        .text_size                = text.size(),
        .time_passed              = time_passed,
    };
}

TestResult Test4Impl() {
    constexpr std::string_view patterns[] = {"a",   "aa",  "aaa", "b",
                                             "bb",  "bbb", "c",   "cc",
                                             "ccc", "d",   "dd",  "ddd"};
    constexpr std::size_t kTextLength     = 1e7;
    constexpr std::size_t kIters          = 1e4;
    std::string text(kTextLength, 'x');

    std::vector<std::pair<std::string_view, size_t>> expected_occurances;
    expected_occurances.reserve(kIters * 6);
    for (std::size_t i = 0; i < kIters; i++) {
        constexpr std::size_t kScale    = kTextLength / (2 * kIters);
        std::size_t index               = i * kScale;
        std::size_t pattern_group_index = i % 4;
        const char symbol = static_cast<char>('a' + pattern_group_index);
        text[index]       = symbol;
        text[index + 1]   = symbol;
        text[index + 2]   = symbol;
        std::size_t j     = pattern_group_index * 3;
        expected_occurances.emplace_back(patterns[j + 0], index);
        expected_occurances.emplace_back(patterns[j + 1], index);
        expected_occurances.emplace_back(patterns[j + 0], index + 1);
        expected_occurances.emplace_back(patterns[j + 2], index);
        expected_occurances.emplace_back(patterns[j + 1], index + 1);
        expected_occurances.emplace_back(patterns[j + 0], index + 2);
    }

    const auto [passed, found_occurances_size, time_passed] =
        RunTests(patterns, text, expected_occurances);
    return {
        .passed                   = passed,
        .found_occurances_size    = found_occurances_size,
        .expected_occurances_size = expected_occurances.size(),
        .patterns_size            = std::size(patterns),
        .text_size                = text.size(),
        .time_passed              = time_passed,
    };
}

void RunTestWrapper(std::function<TestResult()> test_functions,
                    std::uint32_t test_number) noexcept {
    try {
        std::cout << "----------------------------------------------------------------\n"
                  << "Running test " << test_number << '\n';
        TestResult result = test_functions();
        auto time_passed = std::chrono::duration_cast<std::chrono::milliseconds>(result.time_passed).count();
        std::cout << "Test " << test_number
                  << (result.passed ? " passed\n" : " failed\n")
                  << "Number of added patterns: " << result.patterns_size
                  << '\n'
                  << "Length of the text to search patterns in: "
                  << result.text_size << " symbols\n"
                  << "Number of expected occurances: "
                  << result.expected_occurances_size << '\n'
                  << "Number of found occurances: "
                  << result.found_occurances_size << '\n'
                  << "Time passed: " << time_passed << "ms\n"
                  << "----------------------------------------------------------------\n";
    } catch (const std::exception& ex) {
        std::cerr << "Test " << test_number
                  << " failed with exception: " << ex.what() << '\n';
    } catch (...) {
        std::cerr << "Test " << test_number
                  << " failed with unknown exception\n";
    }
}

void RunTests() noexcept {
    RunTestWrapper(Test1Impl, 1);
    RunTestWrapper(Test2Impl, 2);
    RunTestWrapper(Test3Impl, 3);
    RunTestWrapper(Test4Impl, 4);
}

}  // namespace AppSpace
