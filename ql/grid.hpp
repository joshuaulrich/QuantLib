/*
 Copyright (C) 2001, 2002 Sadruddin Rejeb

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it under the
 terms of the QuantLib license.  You should have received a copy of the
 license along with this program; if not, please email ferdinando@ametrano.net
 The license is also available online at http://quantlib.org/html/license.html

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/
/*! \file grid.hpp
    \brief Grid classes with useful constructors for trees and finite diffs

    \fullpath
    ql/%grid.hpp
*/

// $Id$

#ifndef quantlib_time_grid_h
#define quantlib_time_grid_h

#include <ql/array.hpp>
#include <ql/errors.hpp>
#include <ql/qldefines.hpp>
#include <ql/types.hpp>

#include <list>
#include <numeric>
#include <vector>

namespace QuantLib {

    //! spatial grid class
    class Grid : public Array {
      public:
        Grid(double center, double dx, Size steps);
    };

    //! time grid class
    class TimeGrid : public std::vector<Time> {
      public:
        TimeGrid() {}
        //! Regularly spaced time-grid
        TimeGrid(Time end, Size steps);
        //! Time grid with mandatory fixed points
        TimeGrid(const std::list<Time>& times, Size steps);

        Size findIndex(Time t) const;

        Time dt(Size i) const;
    };

    //Inline definitions

    inline Grid::Grid(double center, double dx, Size steps) : Array(steps) {
        for (Size i=0; i<steps; i++)
            (*this)[i] = center + (i - steps/2.0)*dx;
    }

    inline TimeGrid::TimeGrid(Time end, Size steps) {
        Time dt = end/steps;
        for (Size i=0; i<=steps; i++)
            push_back(dt*i);
    }

    inline TimeGrid::TimeGrid(const std::list<Time>& times, Size steps)
    : std::vector<Time>(0) {
        Time last = times.back();
        Time dtMax;
        // what's happening here?
        // is the resulting TimeGrid regularly spaced?
        // please add a comment
        if (steps == 0) {
            std::vector<Time> diff;
            std::back_insert_iterator<std::vector<Time> > ii(diff);
            std::adjacent_difference(times.begin(), times.end(), ii);
            if (diff.front()==0.0)
                diff.erase(diff.begin());
            dtMax = *(std::min_element(diff.begin(), diff.end()));
        } else {
            dtMax = last/steps;
        }

        Time begin = 0.0;
        std::list<Time>::const_iterator t;
        for (t = times.begin(); t != times.end(); t++) {
            Time end = *t;
            if (begin == end) continue;
            Size nSteps = (Size)((end - begin)/dtMax + 1.0);
            double dt = (end - begin)/nSteps;
            for (Size n=0; n<nSteps; n++)
                push_back(begin + n*dt);
            begin = end;
        }
        push_back(begin);
    }

    inline Size TimeGrid::findIndex(Time t) const {
        const_iterator result = std::find(begin(), end(), t);
        QL_REQUIRE(result!=end(), "Using inadequate tree");
        return result - begin();
    }

    inline Time TimeGrid::dt(Size i) const {
        return (*this)[i+1]  - (*this)[i];
    }

}

#endif
