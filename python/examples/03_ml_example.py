#!/usr/bin/env python3
"""Пример 3 — простой ML-пайплайн с использованием uproot + scikit-learn.
Этот пример демонстрационный: он пытается найти признаки и метку в дереве.
Usage: python examples/03_ml_example.py /path/to/file.root
"""
import sys
import numpy as np
import uproot
from sklearn.ensemble import RandomForestClassifier
from sklearn.model_selection import train_test_split
from sklearn.metrics import accuracy_score


def main(path):
    with uproot.open(path) as f:
        # взять первое дерево
        tree_name = None
        for k in f.keys():
            if isinstance(f[k], uproot.models.TTree.Model_TTree):
                tree_name = k
                break
        if tree_name is None:
            # fallback: взять первый объект и попробуем
            tree_name = list(f.keys())[0]

        tree = f[tree_name]
        print(f"Использую дерево: {tree_name}")

        # Попробуем выбрать несколько candidate features и label
        candidates = list(tree.keys())
        print("Доступные ветви:", candidates)

        # Простейшая эвристика
        feature_names = [b for b in candidates if b.lower().startswith("f")][:2]
        if len(feature_names) < 2:
            # fallback: любые первые два числовых
            feature_names = candidates[:2]

        label_candidates = [b for b in candidates if "label" in b.lower() or b.lower().startswith("y")]
        if not label_candidates:
            print("Не найдена явная ветка-метка. Добавьте метку в ROOT-файл или используйте другой файл.")
            return

        label_name = label_candidates[0]

        arrays = tree.arrays(feature_names + [label_name], library="np")
        X = np.vstack([arrays[n] for n in feature_names]).T
        y = arrays[label_name]

        X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)

        clf = RandomForestClassifier(n_estimators=50, random_state=42)
        clf.fit(X_train, y_train)
        preds = clf.predict(X_test)
        acc = accuracy_score(y_test, preds)
        print(f"Accuracy: {acc:.3f}")


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python 03_ml_example.py /path/to/file.root")
        sys.exit(1)
    main(sys.argv[1])
