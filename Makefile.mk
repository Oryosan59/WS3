# --- コンパイラとフラグ ---
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -pedantic # C++11標準を使用し、警告を有効にする (他の必要なフラグも追加可能)

# --- Directory Definitions ---
# --- ディレクトリ定義 ---
SRC_DIR = src       # ソースファイル (.cpp) があるディレクトリ
OBJ_DIR = obj       # オブジェクトファイル (.o) を格納するディレクトリ
BIN_DIR = bin       # 最終的な実行ファイルを格納するディレクトリ
INC_DIR = include   # プロジェクト独自のヘッダーファイル (.h) があるディレクトリ

# --- External Library Paths ---
# --- 外部ライブラリパス ---
# コマンドで指定された特定のパスを使用
NAVIGATOR_LIB_PATH ?= $(HOME)/navigator-lib/target/debug

# --- Include Directories ---
# --- インクルードディレクトリ ---
# プロジェクトのインクルードディレクトリと外部ライブラリのインクルードディレクトリを追加
INCLUDES = -I$(INC_DIR) -I$(NAVIGATOR_LIB_PATH)

# --- Library Directories ---
# --- ライブラリディレクトリ ---
LDFLAGS = -L$(NAVIGATOR_LIB_PATH) \
          -Wl,-rpath=$(NAVIGATOR_LIB_PATH) # 実行時リンクのために rpath を設定

# --- Libraries to link ---
# --- リンクするライブラリ ---
# コマンドで指定された特定のライブラリ名を使用
LIBS = -lbluerobotics_navigator -lpthread -lm

# --- Target Executable ---
# --- ターゲット実行ファイル ---
TARGET_NAME = navigator_control # 実行ファイル名
TARGET = $(BIN_DIR)/$(TARGET_NAME) # 実行ファイルのフルパス

# --- Source and Object Files ---
# --- ソースファイルとオブジェクトファイル ---
# SRC_DIR 内のすべての .cpp ファイルを検索
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
# ソースファイル名に基づいて、OBJ_DIR 内のオブジェクトファイル名を生成
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

# --- デフォルトターゲット: 実行ファイルをビルド ---
all: $(TARGET)

# --- Rule to link the executable ---
# --- 実行ファイルをリンクするルール ---
# 依存関係: すべてのオブジェクトファイル ($(OBJS))
# 順序依存: BIN_DIR ディレクトリが存在すること (| $(BIN_DIR))
	$(CXX) $(LDFLAGS) $^ -o $@ $(LIBS)
	@echo "Build complete: $(TARGET)"

# --- Rule to compile source files into object files ---
# Compile .cpp files from SRC_DIR into .o files in OBJ_DIR
# --- ソースファイルをオブジェクトファイルにコンパイルするルール (パターンルール) ---
# ターゲット: OBJ_DIR 内の任意の .o ファイル
# 依存関係: SRC_DIR 内の対応する .cpp ファイル
# 順序依存: OBJ_DIR ディレクトリが存在すること (| $(OBJ_DIR))
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# --- Directory Creation ---
# These targets create the directories if they don't exist
# They are prerequisites for the compile and link rules using Order-only prerequisites (|)
# --- ディレクトリ作成ルール ---
# これらのターゲットは、ディレクトリが存在しない場合に作成する
# コンパイルルールとリンクルールの順序依存の前提条件 (|) として使用される
$(OBJ_DIR):
	@mkdir -p $@

$(BIN_DIR):
	@mkdir -p $@

# --- ビルド成果物を削除するターゲット ---
clean:
	@echo "Cleaning build artifacts..."
	rm -rf $(OBJ_DIR) $(BIN_DIR) # obj と bin ディレクトリを削除
	@echo "Cleaned."

# --- Phony ターゲット (ファイルを表さないターゲット) ---
# all, clean, およびディレクトリ作成ターゲットはファイルではないため、PHONY として宣言する
.PHONY: all clean $(OBJ_DIR) $(BIN_DIR)

# --- 中間ファイルの削除防止 ---
# make がオブジェクトファイル (.o) を中間ファイルと見なして削除しないようにする
.SECONDARY: $(OBJS)
