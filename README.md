# WS3 - Navigator Control System 🚀

![Build](https://img.shields.io/badge/build-passing-brightgreen)  
リアルタイムセンサ制御・スラスター操作・ゲームパッド入力の統合処理を行う C++ プロジェクトです。BlueRobotics Navigator ライブラリを活用し、ROS などを使用せず軽量で組み込みやすい制御系を構築します。

---

## 🗂️ ディレクトリ構成

```plaintext
WS3/
├── Makefile.mk         # Makefile
├── src/                # ソースファイル (.cpp)
│   ├── main.cpp
│   ├── network.cpp
│   ├── gamepad.cpp
│   ├── thruster_control.cpp
│   └── sensor_data.cpp
├── include/            # ヘッダーファイル (.h/.hpp)
│   ├── network.h
│   ├── gamepad.h
│   ├── thruster_control.h
│   └── sensor_data.h
├── obj/                # コンパイル済オブジェクトファイル (.o)
└── bin/                # 実行ファイル (例: navigator_control)
```

### ✅ この構成のメリット
- **役割の分離**：コードとビルド成果物を明確に分けて管理。
- **スケーラビリティ**：モジュール追加時の見通しが良い。
- **クリーンな管理**：`make clean` で obj/bin ディレクトリのみ削除。

---

## 🛠️ ビルド方法

### 🔧 前提条件
- g++ (C++11以降)
- BlueRobotics Navigator ライブラリ (パス: `/home/pi/navigator-lib/target/debug` にあること)

### 🔄 ビルド手順
```bash
make -f Makefile.mk
```

### 🧹 クリーンアップ
```bash
make -f Makefile.mk clean
```

---

## 🎯 実行ファイル
ビルド成功後、以下の実行ファイルが生成されます：

```bash
./bin/navigator_control
```

---

## 🔌 外部ライブラリ

- [BlueRobotics Navigator](https://github.com/bluerobotics/navigator)  
  センサ・PWM出力・スラスター制御を行うためのRustベースライブラリ（C/C++バインディングを使用）

---

## 🧭 作者

**Oryosan59**  
https://github.com/Oryosan59
