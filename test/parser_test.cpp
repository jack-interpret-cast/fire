#include "fire/parser.hpp"

#include <gtest/gtest.h>

TEST(Parser, VisitStructAllMembers)
{
    struct MyStruct
    {
        int x;
        double y;
    };
    MyStruct my_struct{};
    size_t count = 0;
    auto F       = [&count](const auto&) { return count++; };
    visit_struct_with_error(F, my_struct);
    EXPECT_EQ(count, 2);
}

TEST(ValidateConfig, AllGood)
{
    struct Config
    {
        ConfigOption<bool> boolean{"bool"};
        ConfigOption<int> number{
            "number",
            1,
            'n',
        };
        ConfigOption<int> number2{
            "number2",
            1,
            'R',
        };
    };
    EXPECT_FALSE(validate_config_struct(Config{}));
}

TEST(ValidateConfig, DupeLongnameFail)
{
    struct Config
    {
        ConfigOption<bool> boolean{"bool", false};
        ConfigOption<int> number{"number", 1, 'n'};
        ConfigOption<int> number2{"bool", 1, 'R'};
    };
    EXPECT_TRUE(validate_config_struct(Config{}));
}

TEST(ValidateConfig, DupeShortnameFail)
{
    struct Config
    {
        ConfigOption<bool> boolean{"bool", false};
        ConfigOption<int> number{"number", 1, 'n'};
        ConfigOption<int> number2{"bool2", 1, 'n'};
    };
    EXPECT_TRUE(validate_config_struct(Config{}));
}

struct SingleDefault
{
    ConfigOption<std::string> name{"default arg"};
};

TEST(ParseCmdLine, MissingDefaultFails)
{
    const char* argv[] = {"\0"};
    int argc           = sizeof(argv) / 8;
    auto config        = parse_command_line<SingleDefault>(argc, argv);
    EXPECT_FALSE(config.has_value());
}

TEST(ParseCmdLine, SingleDefaultOk)
{
    const char* argv[] = {"\0", "filename"};
    int argc           = sizeof(argv) / 8;
    auto config        = parse_command_line<SingleDefault>(argc, argv);
    EXPECT_TRUE(config.has_value());
}

struct DoubleDefault
{
    ConfigOption<std::string> name1{"default arg"};
    ConfigOption<std::string> name2{"default arg"};
};

TEST(ParseCmdLine, DoubleDefaultOk)
{
    const char* argv[] = {"\0", "filename1", "filename2"};
    int argc           = sizeof(argv) / 8;
    auto config        = parse_command_line<DoubleDefault>(argc, argv);
    ASSERT_TRUE(config.has_value());
    EXPECT_EQ(config->name1.get(), "filename1");
    EXPECT_EQ(config->name2.get(), "filename2");
}

struct ComplexConfig
{
    ConfigOption<bool> boolean{"bool", false};
    ConfigOption<bool> boolshort{"othername", false, 'd'};
    ConfigOption<int> number{"number", 1, 'n', "my number help"};
    ConfigOption<int> number2{"number2", 1, 'R', "my number help"};
    ConfigOption<uint64_t> number4{"number4", 1};
    ConfigOption<std::string> filename{"filename"};
};

TEST(ParseCmdLine, ComplexConfig)
{
    const char* argv[] = {"\0",  "--bool", "-n",  "-123",    "--number4",
                          "567", "-dR",    "890", "filename"};

    int argc = sizeof(argv) / 8;

    auto config = parse_command_line<ComplexConfig>(argc, argv);
    ASSERT_TRUE(config.has_value());
    EXPECT_EQ(config->boolean.get(), true);
    EXPECT_EQ(config->boolshort.get(), true);
    EXPECT_EQ(config->number.get(), -123);
    EXPECT_EQ(config->number2.get(), 890);
    EXPECT_EQ(config->number4.get(), 567);
    EXPECT_EQ(config->filename.get(), "filename");
}