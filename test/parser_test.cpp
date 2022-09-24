#include "fire/parser.hpp"

#include <gtest/gtest.h>

TEST(Parser, VisitStructAllMembers) {
    struct MyStruct {
        int x;
        double y;
    };
    MyStruct my_struct{};
    size_t count = 0;
    auto F = [&count](const auto &) { return count++; };
    visit_struct_with_error(F, my_struct);
    EXPECT_EQ(count, 2);
}

TEST(ValidateConfig, AllGood) {
    struct Config {
        ConfigOption<bool> boolean{{.help = "bool"}};
        ConfigOption<int> number{{.long_name = "number", .short_name = 'n', .default_val = 1}};
        ConfigOption<int> number2{{.long_name = "number2", .short_name = 'R', .default_val = 1}};
    };
    EXPECT_FALSE(validate_config_struct(Config{}));
}

TEST(ValidateConfig, DupeLongnameFail) {
    struct Config {
        ConfigOption<bool> boolean{{.long_name = "bool", .default_val = false}};
        ConfigOption<int> number{{.long_name="number", .short_name = 'n', .default_val=1}};
        ConfigOption<int> number2{{.long_name="bool", .short_name='R', .default_val=1}};
    };
    EXPECT_TRUE(validate_config_struct(Config{}));
}

TEST(ValidateConfig, DupeShortnameFail) {
    struct Config {
        ConfigOption<bool> boolean{{.long_name="bool"}};
        ConfigOption<int> number{{.long_name="number", .short_name='n'}};
        ConfigOption<int> number2{{.long_name="bool2", .short_name='n'}};
    };
    EXPECT_TRUE(validate_config_struct(Config{}));
}


TEST(ParseCmdLine, Enum) {
    enum class MyEnum {
        A,
        B
    };
    struct Temp {
        ConfigOption<MyEnum> name{{.help="This is an enum"}};
    };
    const char *argv[] = {"\0", "b"};
    int argc = sizeof(argv) / 8;
    auto config = parse_command_line<Temp>(argc, argv);
    EXPECT_FALSE(config.has_value());

    const char *argv2[] = {"\0", "B"};
    argc = sizeof(argv2) / 8;
    auto config2 = parse_command_line<Temp>(argc, argv2);
    EXPECT_TRUE(config2.has_value());
}

struct SingleDefault {
    ConfigOption<std::string> name{{.help="default arg"}};
};

TEST(ParseCmdLine, MissingDefaultFails) {
    const char *argv[] = {"\0"};
    int argc = sizeof(argv) / 8;
    auto config = parse_command_line<SingleDefault>(argc, argv);
    EXPECT_FALSE(config.has_value());
}

TEST(ParseCmdLine, SingleDefaultOk) {
    const char *argv[] = {"\0", "filename"};
    int argc = sizeof(argv) / 8;
    auto config = parse_command_line<SingleDefault>(argc, argv);
    EXPECT_TRUE(config.has_value());
}

struct DoubleDefault {
    ConfigOption<std::string> name1{{}};
    ConfigOption<std::string> name2{{.help="default arg"}};
};

TEST(ParseCmdLine, DoubleDefaultOk) {
    const char *argv[] = {"\0", "filename1", "filename2"};
    int argc = sizeof(argv) / 8;
    auto config = parse_command_line<DoubleDefault>(argc, argv);
    ASSERT_TRUE(config.has_value());
    EXPECT_EQ(config->name1.get(), "filename1");
    EXPECT_EQ(config->name2.get(), "filename2");
}

struct ComplexConfig {
    enum class Colour {
        Red, Green, Blue
    };
    ConfigOption<bool> boolean{{.long_name="bool", .default_val=false}};
    ConfigOption<bool> boolshort{{.long_name="othername", .short_name='d', .default_val=false}};
    ConfigOption<int> number{{.long_name="number", .short_name='n', .default_val=1, .help="my number help"}};
    ConfigOption<int> number2{{.long_name="number2", .short_name='R', .default_val=1, .help="my number help"}};
    ConfigOption<uint64_t> number4{{.long_name="number4", .default_val=1}};
    ConfigOption<std::string> filename{{.help="filename"}};
    ConfigOption<Colour> colour{{.long_name="colour"}};
};

TEST(ParseCmdLine, ComplexConfig) {
    const char *argv[] = {"\0", "--bool", "-n", "-123", "--number4",
                          "567", "-dR", "890", "--colour", "Green", "filename"};

    int argc = sizeof(argv) / 8;

    auto config = parse_command_line<ComplexConfig>(argc, argv);
    ASSERT_TRUE(config.has_value());
    EXPECT_EQ(config->boolean.get(), true);
    EXPECT_EQ(config->boolshort.get(), true);
    EXPECT_EQ(config->number.get(), -123);
    EXPECT_EQ(config->number2.get(), 890);
    EXPECT_EQ(config->number4.get(), 567);
    EXPECT_EQ(config->filename.get(), "filename");
    EXPECT_EQ(config->colour.get(), ComplexConfig::Colour::Green);
}