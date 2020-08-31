import json
import sys
import os
from os import path
import altair as alt  # pip install altair | pip install altair_save
import numpy as np  # pip install numpy
import pandas as pd  # pip install pandas
from scipy import stats


class RegressionGenerator:

    '''Useful to test the effect of X over Y
    By default performs linear regression and returns R^2.
    Saves scatterplot.
    If you know what you're doing:
        Can change the degree of the regression polynomial.
        Can use ridge/lasso/log/ regression.
    Else:
        R^2 closer to 1 means there's a strong correlation.

    '''

    def __init__(self, filepath: str):
        self.data = pd.read_csv(filepath, header=0, skiprows=[0], names=[
                                "total_states", "unique_states", "collisions", "resizes", "time"])
        alt.themes.enable('opaque')

    def generate_report(self, base_path, x='total_states', degree=1):
        '''Saves the scatterplot of x vs y for every y in the csv'''
        cols = self.data.columns
        for col in cols:
            if col != x:
                self.generate_graph(x, col, base_path, degree)

    def generate_graph(self, x, y, base_path, degree=1):
        '''Generates a scatterplot of x vs y, also fits a linear regression of degree 'degree' '''
        if x not in self.data.columns or y not in self.data.columns:
            raise AttributeError("Selected metrics are not valid")
        _data = pd.DataFrame({f'{x}': self.data[x], f'{y}': self.data[y]})

        _results = self.polyfit(_data[f'{x}'], _data[f'{y}'], degree)

        RegressionGenerator.print_results(_results, x, y)
        _chart = alt.Chart(_data).mark_point().encode(
            alt.X(f"{x}", type='quantitative'), alt.Y(f"{y}", type='quantitative')).properties(title=f"{x} vs {y}")

        _chart = self.generate_regression(_chart, x, y, degree)

        RegressionGenerator._save_chart(_chart, x, y, base_path)

    @staticmethod
    def print_results(results, x, y):
        print(f"Result test {x} vs {y}")
        for i, j in enumerate(results['polynomial']):
            print(f"x_{i}: {j}")
        print(f"R^2: {results['determination']}")

    def generate_regression(self, chart, x, y, degree=1):
        chart += chart.transform_regression(x, y, order=degree,
                                            method='poly').mark_line(color='red')
        return chart

    def polyfit(self, x, y, degree=1):
        results = {}

        coeffs = np.polyfit(x, y, degree)

        # Polynomial Coefficients
        results['polynomial'] = coeffs.tolist()

        # r-squared
        p = np.poly1d(coeffs)
        # fit values, and mean
        yhat = p(x)
        ybar = np.sum(y)/len(y)
        ssreg = np.sum((yhat-ybar)**2)
        sstot = np.sum((y - ybar)**2)
        results['determination'] = ssreg / sstot
        return results

    @staticmethod
    def _save_chart(chart, x, y, base_path, scale_factor=1.0, _format='html'):
        chart.save(path.join(base_path,f"{x}_vs_{y}.{_format}"))


class HistogramGenerator:

    '''
    Generates histogram of collisions and probing records
    '''

    def __init__(self, filepath: str):
        with open(f'{filepath}', 'r') as filepath:
            lines = [json.loads(line.strip()[:-1])
                     for line in filepath.readlines()]
        self.data = lines
        alt.themes.enable('opaque')

    def generate_report(self, base_folder, probing=False):
        for i in range(len(self.data)):
            self.generate_histogram(i, base_folder, probing=False)
            self.generate_histogram(i, base_folder, probing=True)

    def generate_histogram(self, x, base_folder, probing=False):
        # source = pd.DataFrame({f'{x}': self.data[x]})
        if not probing:
            _y = [i[1] for i in self.data[x]]
        else:
            _y = [i[0] for i in self.data[x]]
        _x = [i for i in range(len(_y))]
        source = pd.DataFrame({'x': _x, 'count': _y})
        base = alt.Chart(source)
        bar = base.mark_bar().encode(
            x=alt.X(f'x', title="Hash table bucket",
                    bin=False,  type='quantitative'),
            y=alt.Y(f'count', title="Count of collisions ", type='quantitative', axis=alt.Axis(tickMinStep=1.0))).properties(title=f"Collision Histogram {x} ").interactive()
        HistogramGenerator._save_chart(bar, x, base_folder, probing)

    @staticmethod
    def _save_chart(chart, x, base_folder, probing, scale_factor=1.0, _format='html'):
        s = 'probing' if probing else 'no_probing'
        chart.save(path.join(base_folder, f"histogram_{x}_{s}.{_format}"))


class HashTester:

    '''
    Tests the distribution of the hash function.
    If you know what you're doing:
        Performs K-S test.
        H_0 = NOT UNIFORM(0,1)
        H_1 = UNIFORM(0,1)
        Alpha = 0.05
    else:
        Useful for plotting a histogram of the hash function.
    '''

    def __init__(self, filepath: str):
        with open(f'{filepath}', 'r') as filepath:
            lines = [int(i.strip()) for i in filepath.readlines()]
        self.data = lines

    def _normalize(self, max_range: int):
        self.data = np.array([i / (max_range)
                              for i in self.data], dtype=np.float64)

    def _kstest(self):
        d, p = stats.kstest(self.data, 'uniform')
        print(
            f'Uniform(0,1) distribution Kolmogorov–Smirnov test results: \n p-value: {p}\n d-value: {d}')
        return d, p

    def test_hash(self, max_range, val=0.05):
        self._normalize(max_range)
        d, p = self._kstest()
        if p < 0.05 or d < 0.05:
            print(
                f"With a {val}-level significance, null hypothesis is REJECTED.")
        else:
            print(
                f"With a {val}-level significance, null hypothesis is ACCEPTED.")

    @staticmethod
    def _save_chart(chart, base_path, scale_factor=1.0, _format='html'):
        chart.save(f"{base_path}_hash_histogram.{_format}")

    def generate_histogram(self, base_path):
        source = pd.DataFrame({f'x': self.data})
        base = alt.Chart(source)
        bar = base.mark_bar().encode(
            x=alt.X(f'x', title="hash(x) (normalized + binned) ",
                    bin=True,  type='quantitative'),
            y=alt.Y(f'count()', title="count of hash(x)", type='quantitative')).properties(title=f"Hash function distribution ").interactive()
        HashTester._save_chart(bar, base_path)

    def generate_report(self, base_path):
        num = (2**64) - 1  # Pon acá el recorrido de tu hash, por default R+
        self._normalize(num)
        self.generate_histogram(base_path)


    
