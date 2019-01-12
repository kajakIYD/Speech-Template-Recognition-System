#define _USE_MATH_DEFINES
#define _SCL_SECURE_NO_WARNINGS  


#include "Cepstrum.hpp"

#include "fileloader.hpp"

#include <algorithm>
#include <iterator>
#include <fstream>
#include <iostream>

using namespace boost::container;
using namespace boost::numeric::ublas;



int Cepstrum::detectedNumber;

std::tuple<std::vector<std::vector<double>>, int> Cepstrum::CalculateCepstrum(std::vector<double> signal)
{
    //Load some samples (for now samples are preprocessed - silent at the
    //end and at the beginning of the record is removed)
    //And learn the "base" - cepstrum coefficients
    //remember to label each example ("one", "two", "three")

    auto record = 0;
    int fpr = 8000;
    int Np = 10;
    int Nc = 12;
    int twind = 30;
    int tstep = 10;
    int Mlen = (twind * 0.001) * fpr;
    int Mstep = (tstep * 0.001) * fpr;

    signal = Cepstrum::silence(signal, fpr);
    //Na tym etapie signal jest dokladnie taki sam jak w MATLABie

    std::tuple<std::vector<std::vector<double>>, int> cepDetected =
    Cepstrum::calculateCxMatrix(signal, Mlen, Mstep, Nc, Np);

    std::vector<std::vector<double>> Cwz; //One element of Cwzr - so Cwz is cepstrum array for one pattern
    std::vector<std::vector<std::vector<double>>> Cwzr = Cepstrum::extractCwzrFromPatterns(patterns);
    std::vector<int> Nwzr = Cepstrum::extractNwzrFromCwzr(Cwzr); //Vector of number of rows in Cwz
    int Nframes; //Number of rows of Cwz -> that means one element of Cwzr


    Cepstrum::detectedNumber = Cepstrum::dtw(std::get<0>(cepDetected), Cwzr, Nwzr);
    Cepstrum::detectedNumber = std::get<1>(patterns[detectedNumber]);
    return cepDetected;
}


std::tuple<std::vector<std::vector<double>>, int> Cepstrum::calculateCxMatrix(std::vector<double> signal, int Mlen, int Mstep, int Nc, int Np)
{
    //Because of inconsistent vector sizes [Matlab - QT]
    std::vector<double> signalBckp = signal;
    signal.clear();
    for (int i = 0; i < signalBckp.size() - 1; ++i)
    {
        signal.push_back(signalBckp[i]);
    }

	std::vector<double> c;
	std::vector<double>cw;
    std::vector<std::vector<double>>Cx;
	int N = signal.size();

	int Nframes = std::floor((N - Mlen) / Mstep + 1);
	CArray result(Nframes);

	std::vector<int> m;

	for (int i = 1; i <= Nc; ++i) m.push_back(i);

    std::vector<double> signalBeforeSubstraction = signal;
    for (int i = 1; i < signal.size(); ++i)
    {
        signal[i] = signal[i] - (0.9735 * signalBeforeSubstraction[i - 1]);
    }
    std::vector<double> signalAfterSubstraction = signal;
	
	for (int nr = 1; nr <= Nframes; ++nr)
	{
        std::vector<int> n;
        int startValue = 1 + (nr - 1) * Mstep - 1;
        int endValue = Mlen + (nr - 1) * Mstep - 1;
		for (int i = startValue; i <= endValue; ++i)
		{
			n.push_back(i);
		}
		
		std::vector<double> bx;
        for (int index = n[0]; index <= n[n.size() - 1]; ++index)
		{
            bx.push_back(signal[index]);
		}
		
        double sum = 0.0;
        for (int i = 0; i < bx.size(); ++i)
        {
            sum += bx[i];
        }

        double mean = sum / (double)bx.size();
        for (int i = 0; i < bx.size(); ++i)
        {
            bx[i] = bx[i] - mean;
        }
        //std::for_each(bx.begin(), bx.end(), [mean](double &element) { element -= mean; }); //Czemu to nie dziala
		
		std::vector<double> hammingVec;
		for (int i = 0; i <= Mlen - 1; ++i)
		{
			double hammingWindowValue = 0.54 - 0.46 * std::cos(2 * M_PI * i / (Mlen - 1));
			hammingVec.push_back(hammingWindowValue);
            bx[i] *= hammingWindowValue;
		}
		
		std::vector<double> r;
		for (int k = 0; k <= Np; ++k)
		{
			std::vector<double> tmpVec1;
			for (int i = 0; i < Mlen - k; ++i)
			{
				tmpVec1.push_back(bx.at(i));
			}

			std::vector<double> tmpVec2;
			for (int i = k; i < Mlen; ++i)
			{
				tmpVec2.push_back(bx.at(i));
			}

			std::vector<double> multiplicationResults;
			for (int i = 0; i < tmpVec1.size(); ++i)
			{
				multiplicationResults.push_back(tmpVec1.at(i) * tmpVec2.at(i));
			}
			double sum = std::accumulate(multiplicationResults.begin(), multiplicationResults.end(), 0.0);
			r.push_back(sum);
		}

		//std::vector<double> rr;
		boost::numeric::ublas::vector<double> rr(r.size() - 1, 0.0);
		for (int i = 0; i < Np; ++i)
		{
			rr(i) = r.at(i + 1);
		}

		int Rcolumns;
		int Rrows;
		std::vector<std::vector<double>> R;
		for (int m = 1; m <= Np; ++m)
		{
			std::vector<double> tmpVec;
			if (m >= 2)
			{
				for (int id = m; id >= 2; --id)
				{
					tmpVec.push_back(r.at(id - 1));
				}
			}
			for (int id = 1; id <= Np - (m - 1); ++id)
			{
				tmpVec.push_back(r.at(id - 1));
			}
			R.push_back(tmpVec);
			Rcolumns = tmpVec.size();
		}
		Rrows = R.size();
		
		matrix<double> R_ToInverse(Rrows, Rcolumns);
		for (int i = 0; i < Rrows; ++i)
		{
			std::vector<double> vec = R.at(i);
			for (int j = 0; j < Rcolumns; ++j)
			{
				R_ToInverse(i, j) = vec.at(j);
			}
		}

		
		matrix<double> R_Inverted = R_ToInverse;

		if (!inverse(R_Inverted))
		{
            std::cout << "zle uwarunkowana macierz";
            std::vector<std::vector<double>> errVec = {{1, 1}, {0, 0}};
			return std::make_tuple(errVec, 0);
		}

		boost::numeric::ublas::vector<double> a_temp;
		a_temp = R_Inverted * rr;
		
		boost::numeric::ublas::vector<double> a(Nc - Np + a_temp.size(), 0.0);
		for (int i = 0; i < a_temp.size(); ++i)
		{
			a(i) = a_temp(i);
		}

		std::vector<double> c(Nc, 0.0);
		c.at(0) = a(0);
		for (int m = 2; m <= Nc; ++m)
		{
			boost::numeric::ublas::vector<int> k(m-1, 0.0);
			for (int i = 1; i < m; ++i)
			{
				k(i-1) = i;
			}
			double sum = 0.0;
			for (auto &it : k)
			{
				sum += c.at(it - 1) * a(m - it - 1) * it / m;
			}
			c.at(m - 1) = a(m - 1) + sum;

			k.clear();
		}

/* ANOTHER WAY OF CALCULATING CEPSTRAL COEFFICIENTS, TOTALLY UNNECESSARY...
 * BUT DEFINITELY amplyfying :)
//		Complex * bxToFft;
//		bxToFft = new Complex[bx.size()];

//		for (int i = 0; i < bx.size(); ++i)
//		{
//			bxToFft[i] = bx.at(i);
//		}
//		CArray data(bxToFft, bx.size());

//		// forward fft
//		Fft::fft(data);

//		data = abs(data);
//		CArray dataLog(data.size());
		
//		int cnt = 0;
//		for (auto &it : data)
//		{
//			dataLog[cnt] = log(it);
//			++cnt;
//		}

//		data = dataLog * dataLog;

		// inverse fft
//		Fft::ifft(data);

//		CArray dataReal(data.size());
//		cnt = 0;
//		for (auto &it : data)
//		{
//			dataReal[cnt] = real(it);
//			++cnt;
//		}

//		data = dataReal;

//		boost::container::vector<Complex> c1;
//		for (auto &it : data)
//		{
//			c1.push_back(it);
//		}
		
//		CArray bxCArray = stdVecToCArray(bx);

		//Unused, but present in Zielinski's book
        //Another way of calculating cepstral coeffs in MATLAB
//        CArray c2 = rCeps(bxCArray);
		
//		CArray cArr(std::complex<double>(0.0, 0.0), c1.size() - 1);

//		for (int i = 0; i < cArr.size(); ++i)
//		{
//			cArr[i] = c1.at(i + 1);
//		}
*/

        std::vector<double> w; //Weights of cepstral coefficients
        for (int i = 1; i <= m.size(); ++i) w.push_back(1 + Np * std::sin(M_PI*m.at(i-1) / Nc) / 2);

        std::vector<double> cw(c.size());
		for (int i = 0; i < c.size(); ++i)
		{
			cw[i] = c[i] * w[i];
		}
		Cx.push_back(cw);
		n.clear();
	}
		
	return std::make_tuple(Cx, Nframes);
}

////TODO: Make it generic
//std::vector<std::vector<double>> Cepstrum::stdVecToCArray(std::vector<double> x)
//{
//	std::vector<std::vector<double>> res(0.0, x.size());
//	for (int i = 0; i < x.size(); ++i)
//	{
//		res[i] = x[i];
//	}
//	return res;
//}

//std::vector<std::vector<double>> Cepstrum::fft_x_abs(std::vector<std::vector<double>> x)
//{
//	Fft::fft(x);
//	x = abs(x);
//	return x;
//}


//std::vector<std::vector<double>> Cepstrum::rCeps(std::vector<std::vector<double>> x)
//{
//	int n = x.size();

//	std::vector<std::vector<double>> fftxabs = fft_x_abs(x);
//	for (int i = 0; i < fftxabs.size(); ++i)
//	{
//		fftxabs[i] = log(fftxabs[i]);
//	}

//	/*if any(fftxabs == 0)
//		error(message('signal:rceps:ZeroInFFT'));
//	end*/
		
//	CArray xhat(0.0, fftxabs.size());
//	for (int i = 0; i < fftxabs.size(); ++i)
//	{
//		xhat[i] = real(fftxabs[i]);
//	}

//	/*Present in algorithm, but for now unused
//	if nargout > 1
//	odd = fix(rem(n, 2));
	
//	wn = [1; 2 * ones((n + odd) / 2 - 1, 1); ones(1 - rem(n, 2), 1); zeros((n + odd) / 2 - 1, 1)];
	
//	% class(yhat)must be the same as that of xhat to ensure precision rules
	
//	yhat = zeros(size(x), class(xhat));
	
//	yhat(:) = real(ifft(exp(fft(wn.*xhat(:)))));
//	end
//	*/
//	return xhat;
//}

int Cepstrum::dtw(std::vector<std::vector<double>> Cx, std::vector<std::vector<std::vector<double>>> Cwzr, std::vector<int> Nwzr)
{
	int Ns = Cx.size();
	int Np = Cx[0].size();

	std::vector<double> glob;

	for (int number = 0; number < Nwzr.size(); ++number)
	{
		int Nw = Nwzr[number];
		double Q = std::round(0.2 * std::max(Ns, Nw));

		boost::numeric::ublas::matrix<double> ones(Ns, Nw);
		for (unsigned i = 0; i < ones.size1(); ++i)
			for (unsigned j = 0; j < ones.size2(); ++j)
				ones(i, j) = std::numeric_limits<double>::max();

		boost::numeric::ublas::matrix<double> d = ones;
		double tg = (Nw - Q) / (Ns - Q);

        std::vector<double> down(Ns);
        std::vector<double> up(Ns);


        std::ofstream myfile ("example.txt");
        if (myfile.is_open())
        {
            //tg
            myfile << "tg: \n";
            myfile << tg << '\n';

            //Q
            myfile << "Q \n";
            myfile << Q << 'n';

            //Nw
            myfile << "Nw \n";
            myfile << Nw << '\n';

            //down
            myfile << "down \n";
            for (int i = 0; i < down.size(); ++i)
            {
                myfile << down[i] << '\n';
            }

            //up
            myfile << "up \n";
            for (int i = 0; i < down.size(); ++i)
            {
                myfile << down[i] << '\n';
            }

            //d
            myfile << "boost::numeric::ublas::matrix<double> d \n";

            //Cwzr
            myfile << "Cwzr \n";
            for (int c = 0; c < Cwzr.size(); ++c)
            {
                myfile << "Cwzr" << c << "\n";
                auto cMat = Cwzr[c];
                for (int i = 0; i < cMat.size(); ++i)
                {
                    auto cVec = cMat[i];
                    for (int j = 0; j < cVec.size(); ++j)
                    {
                        myfile << cVec[j] << ';';
                    }
                }
                myfile << "\n";
            }
            myfile.close();
        }
        else std::cout << "Unable to open file";


        std::cout << "Skonczylem pisac do pliku";

        for (int ns = 1; ns < Ns + 1; ++ns)
		{
            down[ns-1] = std::max(1.0, std::floor(tg * ns - Q * tg));
            up[ns-1] = std::min(static_cast<double>(Nw), std::ceil(tg*ns + Q));
            for (int nw = down[ns-1] - 1; nw < up[ns-1]; ++nw)
			{
                d(ns-1, nw) = Cepstrum::calculateSquareErrorSum(Cx[ns-1], Cwzr[number], ns, Np, nw, number);;
			}
		}

		boost::numeric::ublas::matrix<double> g = d;
		for (int ns = 1; ns < Ns; ++ns)
		{
            g(ns, 0) = g(ns - 1, 0) + d(ns, 0);
		}
		
        for (int nw = 1; nw < Nw; ++nw)
		{
			g(0, nw) = g(0, nw) + d(0, nw);
		}
		for (int ns = 1; ns < Ns; ++ns)
		{
			for (int nw = std::max(down.at(ns), 1.0); nw < up.at(ns); ++nw)
			{
				double dd = d(ns, nw);
				std::vector<double> temp;
				temp.push_back(g(ns - 1, nw) + dd);
				temp.push_back(g(ns - 1, nw - 1) + 2 * dd);
                temp.push_back(g(ns, nw - 1) + dd); //DO TYCH TEMPÓW WRZUCA ZAR?BI?CIE!!!! PÓ?NIEJ SI? TROSZK? ROZJE?D?A
                std::vector<double>::iterator result = std::min_element(std::begin(temp), std::end(temp));
                g(ns, nw) = static_cast<double>(std::distance(std::begin(temp), result));
			}
		}
        glob.push_back((g(Ns - 1, Nw - 1) / std::sqrt(std::pow((Ns), 2.0) + std::pow((Nw), 2.0))));
	}

    std::vector<double>::iterator result = std::min_element(std::begin(glob), std::end(glob));

    int detectedNumber = std::distance(std::begin(glob), result);
    return detectedNumber;
}

std::vector<std::tuple<std::vector<std::vector<double>>, int>> Cepstrum::patterns;

bool Cepstrum::loadPatterns()
{
    FileLoader fl;

    patterns = fl.loadPatterns();

    bool res = (patterns.empty() == false) ? true : false;
    return res;
}

std::vector<std::vector<std::vector<double>>> Cepstrum::extractCwzrFromPatterns( std::vector<std::tuple<std::vector<std::vector<double>>, int>> patterns)
{
    std::vector<std::vector<std::vector<double>>> Cwzr;

    for (int i = 0; i < patterns.size(); ++i)
    {
        std::vector<std::vector<double>> onePattern; //Couple of cepstral coefficients row for one pattern

        std::tuple<std::vector<std::vector<double>>, int> patternTuple = patterns[i];

        int patternNumber = std::get<1>(patternTuple);

        std::vector<std::vector<double>> cepstralCoefficients = std::get<0>(patternTuple);
        for(int j = 0; j < cepstralCoefficients.size(); ++j)
        {
            std::vector<double> oneRow(cepstralCoefficients[j].size()); //One row of cepstrum coefficients
            for (int k = 0; k < cepstralCoefficients[j].size(); ++k)
            {
                oneRow[k] = cepstralCoefficients[j].at(k);
            }
            onePattern.push_back(oneRow);
        }
        Cwzr.push_back(onePattern);
    }
    return Cwzr;
}


std::vector<int> Cepstrum::extractNwzrFromCwzr(std::vector<std::vector<std::vector<double>>> Cwzr)
{
    std::vector<int> Nwzr;

    for (int i = 0; i < Cwzr.size(); ++i)
    {
        Nwzr.push_back(Cwzr[i].size());
    }
    return Nwzr;
}

std::vector<double> Cepstrum::silence(std::vector<double> signal, int fpr)
{

    double dt1 = 0.01;
    int Mlen = std::floor(dt1 * fpr);
    double dt2 = 0.001;
    int Mstep = std::floor(dt2 * fpr);
    double prog = 0.25 * fpr / 8000;

    std::vector<double> x = signal;

    int N = x.size();
    int Nramek = std::floor((N-Mlen)/Mstep + 1);

    std::vector<double> xAbs;
    for (int i = 0; i < x.size(); ++i)
    {
        auto it = x[i];
        xAbs.push_back(std::abs(it));
    }

    std::vector<double>::iterator resultIt = std::max_element(xAbs.begin(), xAbs.end());
    int indexOfMax = std::distance(xAbs.begin(), resultIt);
    double maxAbsX = xAbs[indexOfMax];
    std::vector<double> xn;

    for (int i = 0; i < x.size(); ++i)
    {
        auto it = x[i];
        xn.push_back(it / maxAbsX);
    }

    int nr1 = 0;
    for (nr1 = 0; nr1 < Nramek; ++nr1)
    {
        int firstIndex = nr1 * Mstep;
        int lastIndex = Mlen + nr1 * Mstep;
        std::vector<double> bx;
        double mulRes = 0.0;
        for (int i = firstIndex; i <= lastIndex; ++i)
        {
            bx.push_back(xn[i]);
            mulRes += xn[i] * xn[i];
        }
        if (mulRes >= prog)
        {
            break;
        }
    }

    int nr2 = 0;
    for (nr2 = Nramek - 1; nr2 >= nr1; --nr2)
    {
        int firstIndex = nr2 * Mstep;
        int lastIndex = Mlen + nr2 * Mstep;
        std::vector<double> bx;
        double mulRes = 0.0;
        for (int i = firstIndex; i < lastIndex; ++i)
        {
            bx.push_back(xn[i]);
            mulRes += xn[i] * xn[i];
        }
        if (mulRes >= prog)
        {
            break;
        }
    }

    std::vector<double> y;

    for (int index = nr1 * Mstep; index <= Mlen + nr2 * Mstep; ++index)
    {
        y.push_back(x[index]);
    }

    return y;
}

double Cepstrum::sum = 0.0;
double Cepstrum::calculateSquareErrorSum(std::vector<double> Cx, std::vector<std::vector<double>> Cwzr, int ns, int Np, int nw, int numberWzorca)
{
    //std::vector<double> substracts;
    sum = 0.0;
    for (int i = 0; i < Np; ++i)
    {
        double val = Cx[i] - Cwzr.at(nw)[i];
        //substracts.push_back(val * val);
        sum += val * val;
    }
    return sqrt(sum);
}

