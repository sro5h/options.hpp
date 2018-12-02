#include <map>
#include <vector>
#include <string>

namespace sro5h {

class Options final {
public:
        struct Key {
                Key(char c, const std::string& s);
                Key(char c);
                Key(const std::string& s);

                bool operator<(const Key& other) const;
                std::string str() const;

                const char c;
                const std::string s;
        };

        bool parse(int argc, char* argv[]);

        void add(Key key, std::size_t valueCount = 0);
        bool has(const Key& key) const;
        std::size_t count(const Key& key) const;
        std::string value(const Key& key) const;
        std::vector<std::string> values(const Key& key) const;
        std::vector<std::string> actions() const;

private:
        struct Expectation {
                explicit Expectation(std::size_t valueCount);

                std::size_t valueCount;
        };

        bool isOption(const std::string& token) const;
        bool isShortOption(const std::string& token) const;
        bool isLongOption(const std::string& token) const;

        bool parseOption(const std::string& token);
        Key parseKey(const std::string& token) const;
        std::string parseValue(const std::string& token) const;

        std::map<Key, Expectation> mExpectations;
        std::map<Key, std::vector<std::string>> mParsed;
        std::vector<std::string> mActions;
};

}

#ifdef OPTIONS_IMPLEMENTATION

#include <iostream>
#include <algorithm>

namespace sro5h {

bool Options::parse(int argc, char* argv[]) {
        bool result = true;
        std::vector<std::string> args(argv + 1, argv + argc);

        for (const std::string& token: args) {
                if (isOption(token)) {
                        result = parseOption(token) && result;

                } else {
                        mActions.push_back(token);
                }
        }

        return result;
}

void Options::add(Key key, std::size_t valueCount) {
        mExpectations.insert({ key, Expectation(valueCount) });
}

bool Options::has(const Key& key) const {
        return mParsed.find(key) != mParsed.end();
}

std::size_t Options::count(const Key& key) const {
        auto itr = mParsed.find(key);

        if (itr != mParsed.end()) {
                return itr->second.size();
        }

        return 0;
}

std::string Options::value(const Key& key) const {
        auto itr = mParsed.find(key);

        if (itr != mParsed.end() && itr->second.size() == 1) {
                return itr->second[0];
        }

        return "";
}

std::vector<std::string> Options::values(const Key& key) const {
        auto itr = mParsed.find(key);

        if (itr != mParsed.end()) {
                return itr->second;
        }

        return std::vector<std::string>();
}

std::vector<std::string> Options::actions() const {
        return mActions;
}

bool Options::isOption(const std::string& token) const {
        return isShortOption(token) || isLongOption(token);
}

bool Options::isShortOption(const std::string& token) const {
        return (token.size() > 1 && token[0] == '-' && token[1] != '-');
}

bool Options::isLongOption(const std::string& token) const {
        return (token.size() > 2 && token[0] == '-' && token[1] == '-');
}

bool Options::parseOption(const std::string& token) {
        const Key parsedKey = parseKey(token);

        const auto itr = std::find_if(mExpectations.begin(), mExpectations.end(),
                        [&parsedKey](const std::pair<Key, Expectation>& pair) -> bool {
                return parsedKey.c == pair.first.c || parsedKey.s == pair.first.s;
        });

        if (itr == mExpectations.end()) {
                std::cout << "Unknown option " << parsedKey.str() << std::endl;
                return false;
        }

        const Key& key = itr->first;
        const Expectation& expectation = itr->second;

        std::string value = parseValue(token);
        if (value.empty()) {
                if (expectation.valueCount == 0) {
                        mParsed.insert({ key, std::vector<std::string>() });
                        return true;

                } else {
                        std::cout << "Option " << key.str() << " expects a value" << std::endl;
                        return false;
                }

        } else {
                if (expectation.valueCount > 0) {
                        // Only allow a maximum of expectation.valueCount values
                        if (mParsed[key].size() < expectation.valueCount) {
                                mParsed[key].push_back(value);
                                return true;
                        }

                        std::cout << "Option " << key.str() << " only expects "
                                  << expectation.valueCount << " values" << std::endl;
                        return false;

                } else {
                        std::cout << "Option " << key.str() << " doesn't expect a value"
                                  << std::endl;
                        return false;
                }
        }
}

Options::Key Options::parseKey(const std::string& token) const {
        if (isShortOption(token)) {
                return Key(token[1]);
        }

        std::size_t count = token.find('=');
        if (count != std::string::npos) {
                // Remove the start index
                count -= 2;
        }

        return Key(token.substr(2, count));
}

std::string Options::parseValue(const std::string& token) const {
        if (isShortOption(token) && token.size() > 2) {
                return token.substr(2);
        }

        std::size_t position = token.find('=');
        if (isLongOption(token) && position != std::string::npos) {
                return token.substr(position + 1);
        }

        return std::string();
}

Options::Key::Key(char c, const std::string& s)
        : c(c)
        , s(s) {
}

Options::Key::Key(char c)
        : c(c)
        , s() {
}

Options::Key::Key(const std::string& s)
        : c(0)
        , s(s) {
}

bool Options::Key::operator<(const Key& other) const {
        if (c < other.c) return true;
        if (c > other.c) return false;

        if (s < other.s) return true;
        if (s > other.s) return false;

        return false;
}

std::string Options::Key::str() const {
        if (c == 0) {
                return std::string("--") + s;
        }

        if (s.empty()) {
                return std::string("-") + c;
        }

        return std::string("-") + c + std::string(", --") + s;
}

Options::Expectation::Expectation(std::size_t valueCount)
        : valueCount(valueCount) {
}

}

#endif
