#include <iostream>
#include <boost/bimap.hpp>
#include <string>
#include <boost/flyweight.hpp>

typedef uint32_t key;

struct User{
    User(const std::string& first_name, const std::string& last_name): first_name{add(first_name)}
    , last_name{add(last_name)}{}
    protected:
    key first_name , last_name;
    static boost::bimap<key, std::string> names;
    static key seed;
    static key add(const std::string& s){
        auto it=names.right.find(s);
        if(it==names.right.end())
        {
            //add it
            names.insert({++seed, s}); 
            return seed;
        }
        return it->second;
    }

    const std::string& get_first_name() const{
        return names.left.find(last_name)->second;
    }

    const std::string& get_last_name() const{
        return names.left.find(last_name)->second;
    }

    friend std::ostream& operator<<(std::ostream& os, const User& obj){
        return os<<"first_name:" <<obj.get_first_name() 
                << " last_name: " <<obj.get_last_name();
    }
};

struct User2{
    boost::flyweight<std::string> first_name, last_name;

    User2(const std::string& first_name, const std::string& last_name): first_name{first_name}, last_name{last_name}{}

};

class FormattedText{
    std::string plainText;
    bool*caps;
public:
    explicit FormattedText(const std::string& plainText): plainText{plainText}{
        caps = new bool[plainText.length()];
    }
    ~FormattedText(){
        delete[] caps;
    }

    void capitalize(int start, int end)
    {   
    for(int i=start; i<=end; ++i)
        caps[i]=true;
    }

    friend std::ostream& operator<<(std::ostream& os, const FormattedText& obj){
        std::string s;
        for(int i=0;i<obj.plainText.length(); ++i){
            char c=obj.plainText[i];
            s += (obj.caps[i] ? toupper(c) : c);
        }
        return os << s;
    }
};

class BetterFormattedText{
public:
    explicit BetterFormattedText(const std::string& plainText): plain_text{plainText}{
    }
    struct TextRange{
        int start, end;
        bool capitalize;
        //other options here eg bold italic , etc

        bool covers(int position) const{
            return position >= start && position <=end;
        }
    };

    TextRange& get_range(int start, int end)
    {
        formatting.emplace_back(TextRange{start, end});
        return *formatting.rbegin();
    }

    friend std::ostream& operator<<(std::ostream& os, const BetterFormattedText& obj){
        std::string s;
        for (size_t i = 0; i < obj.plain_text.length(); i++)
        {
            auto c = obj.plain_text[i];
            for (const auto& rng : obj.formatting)
            {
                if (rng.covers(i) && rng.capitalize)
                    c = toupper(c);
                s += c;
            }
        }
            return os << s;
    }
    private:
    std::string plain_text;
    std::vector<TextRange> formatting;
};




int main(){
    FormattedText ft("This is a brave new world");
    ft.capitalize(10, 15);
    std::cout << ft << std::endl;
    User2 john_doe{"John", "Doe"};
    User2 jane_doe{"Jane", "Doe"};
    std::cout << std::boolalpha <<(&jane_doe.last_name.get()==&john_doe.last_name.get());
    BetterFormattedText bft("This is a brave new world");
    bft.get_range(10, 15).capitalize = true;
    std::cout << bft << std::endl;
}
