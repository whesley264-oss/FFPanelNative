#!/bin/bash

# ============================================
# FF Panel Pro - Build Script
# ============================================

echo "=========================================="
echo "  FF Panel Pro - Build APK"
echo "=========================================="
echo ""

# Cores
VERDE='\033[0;32m'
AMARELO='\033[1;33m'
VERMELHO='\033[0;31m'
SEM_COR='\033[0m'

# Verifica se Android SDK está configurado
if [ -z "$ANDROID_HOME" ]; then
    if [ -d "$HOME/Android/Sdk" ]; then
        export ANDROID_HOME="$HOME/Android/Sdk"
    elif [ -d "/usr/local/android-sdk" ]; then
        export ANDROID_HOME="/usr/local/android-sdk"
    fi
fi

if [ -z "$ANDROID_HOME" ]; then
    echo -e "${VERMELHO}✗ Android SDK não encontrado!${SEM_COR}"
    echo ""
    echo "Por favor, configure o ANDROID_HOME ou instale o Android SDK"
    echo "Download: https://developer.android.com/studio"
    exit 1
fi

echo -e "${VERDE}✓ Android SDK: $ANDROID_HOME${SEM_COR}"
echo ""

# Navega para o diretório do projeto
cd "$(dirname "$0")"

# Verifica se gradlew existe
if [ ! -f "gradlew" ]; then
    echo -e "${AMARELO}⚠ gradlew não encontrado. Baixando...${SEM_COR}"
    
    # Cria gradle wrapper properties
    mkdir -p gradle/wrapper
    
    cat > gradle/wrapper/gradle-wrapper.properties << 'EOF'
distributionBase=GRADLE_USER_HOME
distributionPath=wrapper/dists
distributionUrl=https\://services.gradle.org/distributions/gradle-8.2-bin.zip
networkTimeout=10000
zipStoreBase=GRADLE_USER_HOME
zipStorePath=wrapper/dists
EOF
    
    # Gera gradlew
    gradle wrapper --gradle-version 8.2
fi

# Permissão
chmod +x gradlew

echo "Compilando APK..."
echo ""

# Build
./gradlew assembleDebug --stacktrace

# Verifica resultado
if [ $? -eq 0 ]; then
    echo ""
    echo "=========================================="
    echo -e "${VERDE}✓ Build concluído com sucesso!${SEM_COR}"
    echo "=========================================="
    echo ""
    echo "APK gerado em:"
    echo "  app/build/outputs/apk/debug/app-debug.apk"
    echo ""
    echo "Para instalar no dispositivo:"
    echo "  adb install app/build/outputs/apk/debug/app-debug.apk"
    echo ""
else
    echo ""
    echo "=========================================="
    echo -e "${VERMELHO}✗ Build falhou!${SEM_COR}"
    echo "=========================================="
    echo ""
    echo "Verifique os erros acima e tente novamente."
    exit 1
fi
