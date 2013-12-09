// Copyright (C) 2013 Statoil ASA, Norway.
//
// The file 'canvas_plot_circle.js' is part of ERT - Ensemble based Reservoir Tool.
//
// ERT is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// ERT is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.
//
// See the GNU General Public License at <http://www.gnu.org/licenses/gpl.html>
// for more details.



function CanvasCircle() {
    var X = function (d) { return d; };
    var Y = function (d) { return d; };
    var style = STYLES["default"];
    var radius = 2.5;

    function render(context, x, y) {
        context.lineWidth = style["stroke_width"];
        context.strokeStyle = style["stroke"];
        context.fillStyle = style["fill"];

        context.beginPath();
        context.arc(X(x), Y(y), radius, 0, 2 * Math.PI);
        context.stroke();
    }

    render.x = function (value) {
        if (!arguments.length) return X;
        X = value;
        return render;
    };

    render.y = function (value) {
        if (!arguments.length) return Y;
        Y = value;
        return render;
    };

    render.style = function (value) {
        if (!arguments.length) return style;
        style = value;
        return render;
    };

    return render;
}