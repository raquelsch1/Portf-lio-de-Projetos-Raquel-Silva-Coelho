import streamlit as st
import pandas as pd
import joblib
from custom_transformers import IQRClipper, FeatureSelector
from sklearn.metrics import classification_report

st.set_page_config(page_title="Credit Default Demo", layout="wide")

st.title("Previsão de Inadimplência — Demo")

st.write(
    "Faça upload de um dataset CSV e escolha o modelo para prever "
    "o risco de inadimplência dos clientes."
)

TARGET_COL = "default.payment.next.month"

@st.cache_resource
def carregar_modelos():
    return {
        "Regressão Logística (Base)": joblib.load("lr_baseline.joblib"),
        "Ensemble (Base)": joblib.load("ensemble_baseline.joblib"),
        "Regressão Logística (Melhorada)": joblib.load("lr_melhorado.joblib"),
        "Ensemble (Melhorado)": joblib.load("ensemble_melhorado.joblib"),
    }

modelos = carregar_modelos()

modelo_nome = st.selectbox(
    "Selecione o modelo",
    list(modelos.keys())
)

modelo = modelos[modelo_nome]

threshold = st.slider(
    "Limiar de decisão",
    min_value=0.1,
    max_value=0.9,
    value=0.56,
    step=0.01
)

uploaded_file = st.file_uploader(
    "Faça upload do dataset de teste (CSV)",
    type=["csv"]
)

if uploaded_file is not None:
    df = pd.read_csv(uploaded_file)

    st.subheader("Prévia do dataset")
    st.dataframe(df.head())

    if TARGET_COL in df.columns:
        y_real = df[TARGET_COL]
        X = df.drop(columns=[TARGET_COL])
        st.info("Coluna alvo detectada. Será usada para avaliação do modelo.")
    else:
        y_real = None
        X = df
        st.warning("Dataset sem coluna alvo. Apenas predição será realizada.")

    if st.button("Executar previsão"):
        probs = modelo.predict_proba(X)[:, 1]
        preds = (probs >= threshold).astype(int)

        df_resultado = X.copy()
        df_resultado["prob_inadimplencia"] = probs
        df_resultado["y_pred"] = preds

        if y_real is not None:
            df_resultado["y_real"] = y_real.values
            df_resultado["acerto"] = df_resultado["y_pred"] == df_resultado["y_real"]

        st.subheader("Resultados da Previsão")
        st.dataframe(df_resultado.head(20))

        if y_real is not None:
            st.subheader("Avaliação do Modelo")
            report = classification_report(
                y_real, preds, output_dict=True, zero_division=0
            )
            st.dataframe(pd.DataFrame(report).transpose())

        st.success("Previsão concluída com sucesso!")
