import ctre;

bool match(std::string_view subject) {
    return ctre::match<"[0-9]+">(subject);
}

int main() {
    return !match("12345");
}