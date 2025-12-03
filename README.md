# 🌌 Star Abyss Frontier (星渊前线)

> 同济大学软件工程 2025 C++ 期末课程项目
> **Cocos2d-x 3.17** | **C++17** | **OOD Architecture**

![Build Status](https://img.shields.io/badge/build-passing-brightgreen)
![Platform](https://img.shields.io/badge/platform-Android%20%7C%20iOS%20%7C%20Windows-blue)
![Language](https://img.shields.io/badge/language-C%2B%2B17-orange)
![License](https://img.shields.io/badge/license-MIT-green)

## 📖 项目概述 (Project Overview)

**Star Abyss Frontier (星渊前线)** 是一款基于 **Cocos2d-x** 引擎开发的纵向卷轴射击游戏 (STG)。
本项目不追求简单的功能堆砌，而是致力于构建一个 **高内聚、低耦合** 的游戏对象系统。我们重点运用了 **C++ 多态 (Polymorphism)**、**封装 (Encapsulation)** 以及 **设计模式** 来管理复杂的实体交互与内存生命周期。

---

## 🏗️ 核心架构设计 (System Architecture)

### 1. 类的继承与多态 (Inheritance & Polymorphism)
本项目摒弃了冗余的 `if-else` 类型判断，通过设计严谨的继承树，利用 C++ **虚函数 (Virtual Functions)** 机制实现游戏逻辑的动态绑定。

#### 实体类层级 (Entity Hierarchy)
所有的游戏对象均继承自 `BaseEntity`，实现了统一的更新与渲染接口：

## 📅 开发日志 (Development Log)

| 日期 (Date) | 作者 (Author) | 更新内容 (Change Log) | 备注 (Note) |
| :--- | :--- | :--- | :--- |
| **2025.12.01** | **宋博文** | ✅ 完成 Cocos2d-x 4.0 环境搭建 (Python 2.7 + CMake)<br>✅ 初始化项目仓库，配置 `.gitignore` | 环境配置攻坚 |
| **2025.12.01** | **董泽福** | ✅ 搜集第一版美术素材 (飞机、背景、子弹)<br>✅ 成功运行 Hello World 示例 | 资源准备 |
| **2025.12.02** | **团队** | 🔄 **重构**：决定放弃 Box2D 物理引擎，转为开发 STG 射击游戏<br>✨ 实现主角飞机跟随手指触摸移动逻辑 | 核心玩法变更 |
| **2025.12.02** | **团队** | 🚧 决定放弃Cocos2d-x 4.0，转而使用Cocos2d-x 3.17
| **2025.12.03** | **宋博文** | ✅ 初步实现菜单功能和飞机飞行功能，编写menuscene
