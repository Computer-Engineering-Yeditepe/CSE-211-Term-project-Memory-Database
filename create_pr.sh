#!/bin/bash

# GitHub Pull Request Oluşturma Scripti
# Kullanım: ./create_pr.sh

REPO="Computer-Engineering-Yeditepe/CSE-211-Term-project-Memory-Database"
BRANCH="query-join-engine-oyku"
BASE="main"
TITLE="Add Query and Join Engine Implementation"

# PR açıklamasını oku
BODY=$(cat PR_DESCRIPTION.md)

# GitHub CLI ile PR oluştur
if command -v gh &> /dev/null; then
    echo "GitHub CLI bulundu, PR oluşturuluyor..."
    gh pr create \
        --repo "$REPO" \
        --base "$BASE" \
        --head "$BRANCH" \
        --title "$TITLE" \
        --body "$BODY"
else
    echo "GitHub CLI bulunamadı."
    echo ""
    echo "Lütfen şu adımları izleyin:"
    echo "1. GitHub CLI'yı kurun: brew install gh"
    echo "2. Authenticate edin: gh auth login"
    echo "3. Bu scripti tekrar çalıştırın: ./create_pr.sh"
    echo ""
    echo "VEYA web arayüzünden PR oluşturun:"
    echo "https://github.com/$REPO/pull/new/$BRANCH"
fi

