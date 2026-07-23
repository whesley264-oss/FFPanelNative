# 🎮 FF Panel Pro - Android C++ Native + ESP

![Build Status](https://github.com/whesley264-oss/FFPanelNative/workflows/Build%20APK/badge.svg)
![Android](https://img.shields.io/badge/platform-Android-green)
![C++](https://img.shields.io/badge/language-C++-orange)

Painel de controle para Free Fire com **lógica 100% em C++** via Android NDK e UI mínima em Kotlin. Compile automaticamente via GitHub Actions! 🚀

---

## ⚡ Funcionalidades

### 🎯 Módulos de Combate
| Módulo | Descrição |
|--------|-----------|
| AIM ASSIST | Mira automática |
| NO RECOIL | Elimina recuo 100% |
| SPEED HACK | +30% velocidade |
| FLY MOD | Modo voar livre |

### 👁️ ESP WALLHACK (Individual)
Cada tipo pode ser **ativado/desativado** independentemente:

| ESP | Descrição |
|-----|-----------|
| 📦 **Box ESP** | Retângulos ao redor dos jogadores |
| 📏 **Line ESP** | Linhas do jogador até você |
| 💀 **Skeleton ESP** | Mostrar ossos do personagem |
| 🏷️ **Name ESP** | Nome + Distância |
| ✨ **Glow/Highlight** | Brilho nos jogadores |
| ◢ **Cornered Box** | Caixas com cantos highlight |

---

## 📥 Download APK

O APK é gerado automaticamente! 

👉 https://github.com/whesley264-oss/FFPanelNative/releases

---

## 🏗️ Arquitetura

```
┌─────────────────────────────────────┐
│         MainActivity.kt             │
│    (UI mínima ~260 linhas Kotlin)  │
├─────────────────────────────────────┤
│      JNI Bridge (Java)              │
│    Renderer.java (~10 linhas)      │
├─────────────────────────────────────┤
│      native-lib.cpp                 │
│   (Lógica C++ ~480 linhas)         │
└─────────────────────────────────────┘
```

---

## 🔧 Compilação Automática

O APK é compilado automaticamente via GitHub Actions!

1. Fork este repositório
2. Faça push para `main`
3. O APK será gerado em **Actions** → **Build APK**

---

## ⚠️ Aviso Legal

Este é um projeto **EDUCACIONAL**. Uso em jogos reais pode violar Termos de Serviço.

---

**Desenvolvido por:** whesley264-oss
