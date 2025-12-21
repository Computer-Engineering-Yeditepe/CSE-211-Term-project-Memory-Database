#!/usr/bin/env python3
"""
GitHub Pull Request Oluşturma Scripti
Kullanım: python3 create_pr_python.py
"""

import json
import subprocess
import sys

REPO = "Computer-Engineering-Yeditepe/CSE-211-Term-project-Memory-Database"
BRANCH = "query-join-engine-oyku"
BASE = "main"
TITLE = "Add Query and Join Engine Implementation"

# PR açıklamasını oku
try:
    with open("PR_DESCRIPTION.md", "r") as f:
        BODY = f.read()
except FileNotFoundError:
    BODY = """# Query + Join Engine Implementation

## Overview
This PR adds the complete query parsing and join engine implementation for the database project.

## Author
Öykü Aksungur

## Changes
- ✅ Query parser for SQL-like queries (SELECT, FROM, WHERE, JOIN, ORDER BY, LIMIT)
- ✅ Join engine with two algorithms:
  - Nested Loop Join (for small tables)
  - Hash Join (for larger tables)
- ✅ Support for all join types: INNER, LEFT, RIGHT, FULL OUTER
- ✅ Query execution engine with WHERE filtering
- ✅ Database class for table management
- ✅ Compatible with existing Table/Row/Cell structures
"""

# GitHub CLI ile PR oluşturmayı dene
try:
    result = subprocess.run(
        ["gh", "pr", "create",
         "--repo", REPO,
         "--base", BASE,
         "--head", BRANCH,
         "--title", TITLE,
         "--body", BODY],
        capture_output=True,
        text=True,
        check=True
    )
    print("✅ Pull Request başarıyla oluşturuldu!")
    print(result.stdout)
except subprocess.CalledProcessError as e:
    print(f"❌ Hata: {e.stderr}")
    print("\nGitHub CLI ile PR oluşturulamadı.")
    print("\nLütfen şu adımları izleyin:")
    print("1. GitHub CLI'yı kurun: brew install gh")
    print("2. Authenticate edin: gh auth login")
    print("3. Bu scripti tekrar çalıştırın: python3 create_pr_python.py")
    print("\nVEYA web arayüzünden PR oluşturun:")
    print(f"https://github.com/{REPO}/pull/new/{BRANCH}")
    sys.exit(1)
except FileNotFoundError:
    print("❌ GitHub CLI bulunamadı.")
    print("\nLütfen şu adımları izleyin:")
    print("1. GitHub CLI'yı kurun: brew install gh")
    print("2. Authenticate edin: gh auth login")
    print("3. Bu scripti tekrar çalıştırın: python3 create_pr_python.py")
    print("\nVEYA web arayüzünden PR oluşturun:")
    print(f"https://github.com/{REPO}/pull/new/{BRANCH}")
    sys.exit(1)

