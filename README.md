# WS3 - Navigator Control System 🚀

![Build](https://img.shields.io/badge/build-passing-brightgreen)  
リアルタイムセンサ制御・スラスター操作・ゲームパッド入力の統合処理を行う C++ プロジェクトです。BlueRobotics Navigator-lib ライブラリを活用し、ROS などを使用せず軽量で組み込みやすい制御系を構築します。

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
- [BlueRobotics Navigator-lib](https://github.com/bluerobotics/navigator-lib) がビルド済みであること  
  （デフォルトでは `~/navigator-lib/target/debug` にインストールされている想定）

> **注:**  
> ライブラリのパスは環境変数 `NAVIGATOR_LIB_PATH` で上書き可能です：
>
> ```bash
> make -f Makefile.mk NAVIGATOR_LIB_PATH=/your/custom/path
> ```
>
> 毎回打つのが面倒な場合は、環境変数に書いておくと便利です：
>
> ```bash
> export NAVIGATOR_LIB_PATH=/your/custom/path
> make -f Makefile.mk
> ```


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

- [BlueRobotics Navigator-lib](https://github.com/bluerobotics/navigator-lib)  
  センサ・PWM出力・スラスター制御を行うためのRustベースライブラリ（C/C++バインディングを使用）

---

## 🎁 オマケ：Navigator-lib & Flight Controller のセットアップガイド

このプロジェクトで使用している [BlueRobotics Navigator-lib](https://github.com/bluerobotics/navigator-lib) のバインディング構築方法と、Raspberry Pi 上で Navigator Flight Controller を使うための準備手順を紹介します🛠️✨

---

### 🚀 1. Raspberry Pi OS の準備

#### 1.1 OS イメージのダウンロード  
推奨イメージ（Lite版）:

| アーキテクチャ | ファイル名 | サイズ |
|----------------|------------|--------|
| 32-bit (armhf) | `2023-02-21-raspios-bullseye-armhf-lite.img.xz` | 約362MB |
| 64-bit (arm64) | `2023-02-21-raspios-bullseye-arm64-lite.img.xz` | 約307MB |

🔗 [Raspberry Pi OS公式サイト](https://www.raspberrypi.com/software/operating-systems/)
🔗 [2023-02-21-raspios-bullseye-armhf-lite.img.xz](https://downloads.raspberrypi.com/raspios_lite_armhf/images/raspios_lite_armhf-2023-02-22/)
🔗 [2023-02-21-raspios-bullseye-arm64-lite.img.xz](https://downloads.raspberrypi.com/raspios_lite_arm64/images/raspios_lite_arm64-2023-02-22/)

#### 1.2 Raspberry Pi Imager で microSD にフラッシュ  
- SSH有効化  
- ホスト名、ユーザー名、Wi-Fi設定をプリセット  
- 固定IPの設定例:
  ```bash
  sudo nano /etc/dhcpcd.conf
  ```

最下に記入
  ```conf
  interface eth0
  static ip_address=192.168.1.100/24
  ```

#### 1.3 必要なツールのインストール

```bash
sudo apt update
sudo apt install git i2c-tools
```

---

### 🧭 2. Navigator ハードウェアのセットアップ

#### 2.1 オーバーレイ設定スクリプトの実行

```bash
sudo su -c 'curl -fsSL https://raw.githubusercontent.com/bluerobotics/blueos-docker/master/install/boards/configure_board.sh | bash'
sudo reboot
```

---

### 🧪 3. Navigator-lib のビルドと実行

#### 3.1 必要パッケージのインストール

```bash
sudo apt install cmake git
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
source "$HOME/.cargo/env"
```

#### 3.2 ライブラリのクローンと C++ バインディングのビルド

```bash
git clone https://github.com/bluerobotics/navigator-lib.git
cd navigator-lib/examples/cpp
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug --parallel
```

#### 3.3 サンプル実行ファイル

```bash
./build/simple
./build/rainbow
```

> **備考**: C++バインディングのビルド結果は `navigator-lib/target/debug` に配置されるため、`Makefile` で `NAVIGATOR_LIB_PATH` にこのパスを指定して利用します。

---

## 🧭 作者

**Oryosan59**  
https://github.com/Oryosan59
