# 🌌 Star Abyss Frontier (星渊前线)

> 同济大学软件工程 2025 C++ 期末课程项目
> **Cocos2d-x 4.0** | **C++17** | **OOD Architecture**

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

```text
BaseEntity (Abstract Class)
├── BasePlane (飞机基类)
│   ├── PlayerPlane (封装了单例模式与触控逻辑)
│   └── BaseEnemy (敌机基类)
│       ├── KamikazeEnemy (行为：直线冲撞)
│       ├── ShootingEnemy (行为：定点射击)
│       └── BossEnemy (行为：多阶段状态机)
└── BaseBullet (子弹基类)
    ├── StraightBullet (线性弹道)
    └── TrackingBullet (追踪算法)
