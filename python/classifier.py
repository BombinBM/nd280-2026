import argparse
import os
import glob

import pandas as pd
import numpy as np

from sklearn.model_selection import train_test_split
from sklearn.ensemble import GradientBoostingClassifier
from sklearn.metrics import (
    classification_report, confusion_matrix, roc_auc_score, 
    accuracy_score, precision_score, recall_score, f1_score
)
from sklearn.pipeline import Pipeline
from sklearn.preprocessing import StandardScaler
import joblib
import matplotlib.pyplot as plt


def load_csvs(paths):
    files = []
    for p in paths:
        if os.path.isdir(p):
            files.extend(glob.glob(os.path.join(p, "*.csv")))
        else:
            files.append(p)
    files = [f for f in files if os.path.exists(f)]
    if not files:
        raise FileNotFoundError("No csv files found in given paths")
    df = pd.concat([pd.read_csv(f) for f in files], ignore_index=True)
    return df


def prepare_dataset(df):
    # Expect PDG column
    if "PDG" not in df.columns:
        raise ValueError("Input CSV must contain 'PDG' column")

    # Map labels: protons (2212) -> 0, antimuons (-13) -> 1, pions+ (221) -> 2
    def map_label(x):
        try:
            v = int(x)
        except Exception:
            return np.nan
        if v == 2212:
            return 0
        if v == -13:
            return 1
        if v == 211:
            return 2
        return np.nan

    df = df.copy()
    df["label"] = df["PDG"].apply(map_label)
    df = df.dropna(subset=["label"])  # keep only proton/muon rows
    df["label"] = df["label"].astype(int)

    # Choose features that exist in the CSV
    candidates = [
        "entered_hits",
        "total_charge",
        "average_charge",
        # "min_time",
        # "max_time",
    ]
    features = [c for c in candidates if c in df.columns]
    if not features:
        raise ValueError(f"None of the expected features found. Checked: {candidates}")

    X = df[features].fillna(0.0)
    y = df["label"].values
    return X, y, features


def plot_metrics_vs_estimators(pipeline, X_train, y_train, X_test, y_test, plot_prefix):
    """Plot metrics vs number of estimators."""
    gb = pipeline.named_steps["gb"]
    scaler = pipeline.named_steps["scaler"]
    X_train_scaled = scaler.transform(X_train)
    X_test_scaled = scaler.transform(X_test)
    train_accuracies = []
    test_accuracies = []
    test_precisions = []
    test_recalls = []
    test_f1s = []

    for preds_train in gb.staged_predict(X_train_scaled):
        train_accuracies.append(accuracy_score(y_train, preds_train))

    for preds_test in gb.staged_predict(X_test_scaled):
        test_accuracies.append(accuracy_score(y_test, preds_test))
        test_precisions.append(precision_score(y_test, preds_test, zero_division=0, average="macro"))
        test_recalls.append(recall_score(y_test, preds_test, zero_division=0, average="macro"))
        test_f1s.append(f1_score(y_test, preds_test, zero_division=0, average="macro"))

    n_estimators_range = range(1, len(test_accuracies) + 1)

    fig, axes = plt.subplots(2, 2, figsize=(12, 9))

    # Accuracy
    axes[0, 0].plot(n_estimators_range, train_accuracies, label="Train", marker='o', markersize=3)
    axes[0, 0].plot(n_estimators_range, test_accuracies, label="Test", marker='s', markersize=3)
    axes[0, 0].set_xlabel("Number of Estimators")
    axes[0, 0].set_ylabel("Accuracy")
    axes[0, 0].set_title("Accuracy vs Estimators")
    axes[0, 0].legend()
    axes[0, 0].grid(True, alpha=0.3)

    # Precision
    axes[0, 1].plot(n_estimators_range, test_precisions, label="Precision", marker='o', markersize=3, color='green')
    axes[0, 1].set_xlabel("Number of Estimators")
    axes[0, 1].set_ylabel("Precision")
    axes[0, 1].set_title("Precision vs Estimators")
    axes[0, 1].legend()
    axes[0, 1].grid(True, alpha=0.3)

    # Recall
    axes[1, 0].plot(n_estimators_range, test_recalls, label="Recall", marker='o', markersize=3, color='orange')
    axes[1, 0].set_xlabel("Number of Estimators")
    axes[1, 0].set_ylabel("Recall")
    axes[1, 0].set_title("Recall vs Estimators")
    axes[1, 0].legend()
    axes[1, 0].grid(True, alpha=0.3)

    # F1-Score
    axes[1, 1].plot(n_estimators_range, test_f1s, label="F1-Score", marker='o', markersize=3, color='red')
    axes[1, 1].set_xlabel("Number of Estimators")
    axes[1, 1].set_ylabel("F1-Score")
    axes[1, 1].set_title("F1-Score vs Estimators")
    axes[1, 1].legend()
    axes[1, 1].grid(True, alpha=0.3)

    plt.tight_layout()
    plt.savefig(f"{plot_prefix}_metrics_vs_estimators.png", dpi=100)
    print(f"Saved metrics plot to {plot_prefix}_metrics_vs_estimators.png")
    plt.close()


# def plot_decision_boundary(pipeline, X, y, feature_names, plot_prefix):
    # """Plot 2D decision boundary using first two features."""
    # if X.shape[1] < 2:
    #     print("Cannot plot decision boundary with less than 2 features")
    #     return

    # # Use first two features
    # X_2d = X.iloc[:, :2].values
    # feature_pair = feature_names[:2]

    # # Create mesh
    # x_min, x_max = X_2d[:, 0].min() - 1, X_2d[:, 0].max() + 1
    # y_min, y_max = X_2d[:, 1].min() - 1, X_2d[:, 1].max() + 1
    # h = (x_max - x_min) / 100
    # xx, yy = np.meshgrid(np.arange(x_min, x_max, h), np.arange(y_min, y_max, h))

    # # Create full feature matrix for prediction (padding with zeros for extra features)
    # mesh_points = np.c_[xx.ravel(), yy.ravel()]
    # if X.shape[1] > 2:
    #     padding = np.zeros((mesh_points.shape[0], X.shape[1] - 2))
    #     mesh_points = np.hstack([mesh_points, padding])

    # # Convert to DataFrame with feature names to avoid sklearn warning
    # mesh_df = pd.DataFrame(mesh_points, columns=list(X.columns))

    # # Predict on mesh
    # Z = pipeline.predict(mesh_df)
    # Z = Z.reshape(xx.shape)

    # # Plot
    # plt.figure(figsize=(10, 8))
    # plt.contourf(xx, yy, Z, alpha=0.3, levels=1, colors=['red', 'blue'])
    # plt.contour(xx, yy, Z, levels=[0.5], colors='black', linewidths=2)

    # # Plot data points
    # scatter = plt.scatter(X_2d[y == 0, 0], X_2d[y == 0, 1], c='red', label='Proton (0)', marker='o', edgecolors='k')
    # scatter = plt.scatter(X_2d[y == 1, 0], X_2d[y == 1, 1], c='blue', label='Muon (1)', marker='s', edgecolors='k')

    # plt.xlabel(f"{feature_pair[0]} (normalized)")
    # plt.ylabel(f"{feature_pair[1]} (normalized)")
    # plt.title(f"Decision Boundary: {feature_pair[0]} vs {feature_pair[1]}")
    # plt.legend()
    # plt.tight_layout()
    # plt.savefig(f"{plot_prefix}_decision_boundary.png", dpi=100)
    # print(f"Saved decision boundary plot to {plot_prefix}_decision_boundary.png")
    # plt.close()


def train_and_evaluate(X, y, n_estimators=100, test_size=0.2, random_state=42, out_model=None, plot_prefix=None):
    X_train, X_test, y_train, y_test = train_test_split(
        X, y, test_size=test_size, random_state=random_state, stratify=y if len(np.unique(y)) > 1 else None
    )

    pipeline = Pipeline([
        ("scaler", StandardScaler()),
        ("gb", GradientBoostingClassifier(n_estimators=n_estimators, random_state=random_state, verbose=0)),
    ])

    print("Training model...")
    pipeline.fit(X_train, y_train)

    preds = pipeline.predict(X_test)
    probs = pipeline.predict_proba(X_test) if hasattr(pipeline, "predict_proba") else None

    print("\nClassification report:\n", classification_report(y_test, preds, digits=4))
    print("Confusion matrix:\n", confusion_matrix(y_test, preds))
    if probs is not None and len(np.unique(y_test)) >= 2:
        if plot_prefix:
            from sklearn.metrics import roc_curve, auc
            from sklearn.preprocessing import label_binarize
            from itertools import cycle

            n_classes = len(np.unique(y_test))

            if n_classes > 2:
                y_test_binarized = label_binarize(y_test, classes=np.unique(y_test))

                fpr = dict()
                tpr = dict()
                roc_auc = dict()

                # if len(probs.shape) == 1:
                # # Если probs одномерный, значит это бинарная классификация
                # # Преобразуем в матрицу с двумя столбцами
                #     probs = np.column_stack([1 - probs, probs])

                for i in range(n_classes):
                    fpr[i], tpr[i], _ = roc_curve(y_test_binarized[:, i], probs[:, i])
                    roc_auc[i] = auc(fpr[i], tpr[i])

                all_fpr = np.unique(np.concatenate([fpr[i] for i in range(n_classes)]))
                mean_tpr = np.zeros_like(all_fpr)

                for i in range(n_classes):
                    mean_tpr += np.interp(all_fpr, fpr[i], tpr[i])

                mean_tpr /= n_classes
                fpr["macro"] = all_fpr
                tpr["macro"] = mean_tpr
                roc_auc["macro"] = auc(fpr["macro"], tpr["macro"])

                plt.figure(figsize=(10, 8))
                colors = cycle(['blue', 'red', 'green', 'orange', 'purple', 'brown'])

                for i, color in zip(range(n_classes), colors):
                    plt.plot(fpr[i], tpr[i], color=color, lw=2, label=f"Class {i} (AUC = {roc_auc[i]:.4f})")

                plt.plot(fpr["macro"], tpr["macro"], color="navy", lw=3, linestyle="--", label=f'Macro-average (AUC = {roc_auc["macro"]:.4f})')
                plt.plot([0, 1], [0, 1], 'k--', lw=2)
                plt.xlim([0.0, 1.0])
                plt.ylim([0.0, 1.05])
                plt.xlabel('False Positive Rate')
                plt.ylabel('True Positive Rate')
                plt.title('Multiclass ROC Curves (One-vs-Rest)')
                plt.legend(loc="lower right")
                plt.grid(True, alpha=0.3)

            else:
                auc_score = roc_auc_score(y_test, probs[:, 1])
                fpr, tpr, _ = roc_curve(y_test, probs[:, 1])
                plt.figure()
                plt.plot(fpr, tpr, label=f"AUC={auc_score:.4f}")
                plt.plot([0, 1], [0, 1], "k--")
                plt.xlabel("False Positive Rate")
                plt.ylabel("True Positive Rate")
                plt.title("ROC Curve")
                plt.legend()
                plt.grid(True, alpha=0.3)
            plt.tight_layout()
            plt.savefig(f"{plot_prefix}_roc.png", dpi=100)
            print(f"Saved ROC plot to {plot_prefix}_roc.png")
            plt.close()

            if n_classes > 2:
                print("\nROC AUC scores:")
                for i in range(n_classes):
                    print(f"  Class {i}: {roc_auc[i]:.4f}")
                print(f"  Macro-average: {roc_auc['macro']:.4f}")
    # Additional visualizations
    if plot_prefix:
        print("\nGenerating additional plots...")
        plot_metrics_vs_estimators(pipeline, X_train, y_train, X_test, y_test, plot_prefix)
        # plot_decision_boundary(pipeline, X_train, y_train, list(X.columns), plot_prefix)

    if out_model:
        joblib.dump({"model": pipeline, "features": list(X.columns)}, out_model)
        print(f"Saved model to {out_model}")


def main():
    parser = argparse.ArgumentParser(description="Train Gradient Boosting binary classifier (proton vs muon)")
    parser.add_argument("paths", nargs="+", help="CSV files or directories to read")
    parser.add_argument("--output", "-o", default="python/gb_model.joblib", help="Output model path")
    parser.add_argument("--n-estimators", type=int, default=100)
    parser.add_argument("--test-size", type=float, default=0.2)
    parser.add_argument("--random-state", type=int, default=42)
    parser.add_argument("--plot-prefix", default=None, help="Prefix for saved plots")

    args = parser.parse_args()

    df = load_csvs(args.paths)
    X, y, features = prepare_dataset(df)
    print(f"Training on {len(X)} examples with features: {features}")
    train_and_evaluate(
        X,
        y,
        n_estimators=args.n_estimators,
        test_size=args.test_size,
        random_state=args.random_state,
        out_model=args.output,
        plot_prefix=args.plot_prefix,
    )


if __name__ == "__main__":
    main()
