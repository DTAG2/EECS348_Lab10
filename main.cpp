#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

//1. Read the candidate numbers from the file:
std::vector<std::string> load_num_file(const std::string& filename) {
    std::ifstream infile(filename); //get file object
    std::vector<std::string> numbers; //create numbers vector variable
    std::string line;
    while (std::getline(infile, line)) {
        if (!line.empty())
            numbers.push_back(line); //load all non empty lines into numbers
    }
    return numbers;
}

bool isDigit(char c) { //check if the string is evaulated as being between the two ascii values
    return (c >= '0' && c <= '9');
}

//2. Identify if a number is a valid double number:
bool valid_double(const std::string& s) {

    int n = s.size();
    if (n == 0) return false;
    int i = 0;
    //optional sign
    if (s[i] == '+' || s[i] == '-')
        i++;
    //at least one digit before the decimal (A)
    int startDigits = i;
    while (i < n && isDigit(s[i]))
        i++;
    if (i == startDigits) //if no digits
        return false;
    //optional decimal part
    if (i < n && s[i] == '.') {
        i++;
        int dec = i;
        while (i < n && isDigit(s[i]))
            i++;
        //must have at least one digit after decimal
        if (i == dec)
            return false;
    }
    //must reach end of string
    return (i == n);
}

//3. Perform addition between the parsed double number and another specified double number:
std::string addDoubleStrings(const std::string& sa, const std::string& sb) {
    //handle and strip signs
    bool negA = (!sa.empty() && sa[0] == '-');
    bool negB = (!sb.empty() && sb[0] == '-');

    std::string a;
    std::string b;
    if (sa[0] == '+' || sa[0] == '-') {
        a = sa.substr(1);
    } else {
        a = sa;
    }

    if (sb[0] == '+' || sb[0] == '-') {
        b = sb.substr(1);
    } else {
        b = sb;
    }

    //split into integer and decimal parts
    auto split = [](const std::string& s) {
        size_t pos = s.find('.');
        if (pos == std::string::npos)
            return std::make_pair(s, std::string(""));
        else
            return std::make_pair(s.substr(0, pos), s.substr(pos + 1));
    };

    //get integer and decimal parts of a and b strings
    auto [intA, decA] = split(a);
    auto [intB, decB] = split(b);

    //equalize decimal lengths
    size_t maxDec = std::max(decA.size(), decB.size());
    decA.append(maxDec - decA.size(), '0');
    decB.append(maxDec - decB.size(), '0');

    //equalize integer lengths
    size_t maxInt = std::max(intA.size(), intB.size());
    intA.insert(intA.begin(), maxInt - intA.size(), '0');
    intB.insert(intB.begin(), maxInt - intB.size(), '0');


    //compare magnitudes
    auto cmpMag = [&](const std::string& xInt, const std::string& xDec,
                      const std::string& yInt, const std::string& yDec) {
        if (xInt != yInt)
            return xInt > yInt;
        return xDec > yDec;
    };

    std::string resInt, resDec;
    bool resultNeg = false;

    if (negA == negB) {
        //same sign: perform addition
        int carry = 0;
        resDec.resize(maxDec);
        for (int i = int(maxDec) - 1; i >= 0; --i) {
            int sum = (decA[i] - '0') + (decB[i] - '0') + carry;
            resDec[i] = char('0' + (sum % 10));
            carry = sum / 10;
        }
        resInt.resize(maxInt);
        for (int i = int(maxInt) - 1; i >= 0; --i) {
            int sum = (intA[i] - '0') + (intB[i] - '0') + carry;
            resInt[i] = char('0' + (sum % 10));
            carry = sum / 10;
        }
        if (carry)
            resInt.insert(resInt.begin(), '1');
        resultNeg = negA;
    } else {
        // Different signs: perform subtraction
        bool aLarger = cmpMag(intA, decA, intB, decB);
        std::string bigInt = aLarger ? intA : intB;
        std::string bigDec = aLarger ? decA : decB;
        std::string smallInt = aLarger ? intB : intA;
        std::string smallDec = aLarger ? decB : decA;
        resultNeg = aLarger ? negA : negB;

        // decimal subtraction
        resDec.resize(maxDec);
        int borrow = 0;
        for (int i = int(maxDec) - 1; i >= 0; --i) {
            int diff = (bigDec[i] - '0') - (smallDec[i] - '0') - borrow;
            if (diff < 0) { diff += 10; borrow = 1; }
            else borrow = 0;
            resDec[i] = char('0' + diff);
        }
        // Integer subtraction
        resInt.resize(maxInt);
        for (int i = int(maxInt) - 1; i >= 0; --i) {
            int diff = (bigInt[i] - '0') - (smallInt[i] - '0') - borrow;
            if (diff < 0) { diff += 10; borrow = 1; }
            else borrow = 0;
            resInt[i] = char('0' + diff);
        }
    }

    //trim leading zeros in integer
    auto nz = resInt.find_first_not_of('0');
    if (nz == std::string::npos) resInt = "0";
    else resInt = resInt.substr(nz);
    //trim trailing zeros in decimal
    auto iz = resDec.find_last_not_of('0');
    if (iz == std::string::npos) resDec.clear();
    else resDec = resDec.substr(0, iz + 1);

    //result
    std::string result;
    if (resultNeg && !(resInt == "0" && resDec.empty()))
        result.push_back('-');
    result += resInt;
    if (!resDec.empty()) {
        result.push_back('.');
        result += resDec;
    }
    return result;
}

int main() {
    std::cout << "Enter filename: "; //get file name from user
    std::string filename;
    std::cin >> filename;

    auto numbers = load_num_file(filename); //load the numbers from the file
    const std::string constant = "-123.456"; //declare constant string

    for (const auto& num : numbers) { //looping voer all the numbers in the "numbers" vector
        if (valid_double(num)) { //check if the number is valid
            std::string sum = addDoubleStrings(num, constant); //add the number and double -123.456
            std::cout << num << " + " << constant << " = " << sum << std::endl; //print the result of the addition
        } else {  //if the number is invald
            std::cout << "Invalid number: " << num << std::endl; //print that the number is invalid
        }
    }

    return 0;
}
