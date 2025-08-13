CXX        ?= g++
PKG_CONFIG ?= pkg-config

OPENSSL_LIBS   := $(shell $(PKG_CONFIG) --libs openssl 2>/dev/null)
OPENSSL_CFLAGS := $(shell $(PKG_CONFIG) --cflags openssl 2>/dev/null)
ifeq ($(OPENSSL_LIBS),)
  OPENSSL_LIBS := -lcrypto
endif

CXXFLAGS += -std=c++17 -O2 -pipe \
            -Wall -Wextra -Wpedantic -Wformat -Wformat-security \
            -D_FORTIFY_SOURCE=3 -D_GLIBCXX_ASSERTIONS \
            -fstack-protector-strong -fstack-clash-protection \
            -fcf-protection=full \
            -fPIE -ffunction-sections -fdata-sections
CPPFLAGS += $(OPENSSL_CFLAGS)

LDFLAGS  += -Wl,-O1 -Wl,--as-needed \
            -Wl,-z,relro -Wl,-z,now -Wl,-z,noexecstack \
            -pie -Wl,--gc-sections

SRC    := main.cpp Utils.cpp
OBJ    := $(SRC:.cpp=.o)
TARGET := IDAKeyGen

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(LDFLAGS) $^ $(OPENSSL_LIBS) -o $@

%.o: %.cpp license_utils.hpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	$(RM) $(OBJ) $(TARGET)

debug: CXXFLAGS := $(CXXFLAGS) -Og -g3
debug: clean all

.PHONY: all run clean debug
