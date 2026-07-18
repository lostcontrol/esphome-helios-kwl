import re

with open("components/helios_kwl/helios_kwl.cpp") as f:
    content = f.read()

# Fix C-style cast properly
content = re.sub(
    r"uint8_t mask = s == 0 \? 0 : \(uint8_t\)\(\(1u << std::min\(static_cast<uint8_t>\(8\), s\)\) - 1u\);",
    r"uint8_t mask = s == 0 ? 0 : static_cast<uint8_t>((1u << std::min(static_cast<uint8_t>(8), s)) - 1u);",
    content
)

with open("components/helios_kwl/helios_kwl.cpp", "w") as f:
    f.write(content)
