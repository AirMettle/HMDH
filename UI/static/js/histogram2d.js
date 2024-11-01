let processedData = [];
const Labels = [
    "NaN",
    "+&infin;", // Larger infinity sign
    "+x<sup>+y</sup>", // Using superscript for exponent
    "+x<sup>-y</sup>", // Using superscript for exponent
    "0",
    "-x<sup>-y</sup>", // Using superscript for exponent
    "-x<sup>+y</sup>", // Using superscript for exponent
    "-&infin;" // Larger infinity sign
];
const Labels_sup = ["NAN", "+Inf", "+#, +Exp", "+#, -Exp", "0", "-#, -Exp", "-#, +Exp", "-Inf"];
let positiveExponentMap = {};
let negativeExponentMap = {};
let positiveExponentData = [];
let negativeExponentData = [];
const LabelsSVG = [
    { text: "NaN", type: "normal" },
    { text: "+∞", type: "normal" },
    { text: "+x", type: "normal", superscript: "+y" },
    { text: "+x", type: "normal", superscript: "-y" },
    { text: "0", type: "normal" },
    { text: "-x", type: "normal", superscript: "-y" },
    { text: "-x", type: "normal", superscript: "+y" },
    { text: "-∞", type: "normal" }
];

// Function to load processed JSON data
function resetPage() {
    location.reload();
}
function showLegend() {
    const overlay = document.getElementById('legend-overlay');
    const table = overlay.querySelector('table');
    // Clear existing rows (if any)
    while (table.rows.length > 1) {
        table.deleteRow(1);
    }
    // Insert rows for legend
    const labels = ["000", "001", "010", "011", "100", "101", "110", "111"];
    const descriptions = [
        "Not a Number (Special)",
        "Positive infinity (Special)",
        "Positive Number, positive exponent",
        "Positive Number, negative exponent",
        "Zero",
        "Negative Number, negative exponent",
        "Negative Number, positive exponent",
        "Negative Infinity (Special)"
    ];
    labels.forEach((label, index) => {
        const row = table.insertRow();
        const cell1 = row.insertCell(0);
        const cell2 = row.insertCell(1);
        cell1.textContent = label;
        cell2.textContent = descriptions[index];
    });

    overlay.style.display = 'block';
}

function closeLegend() {
    document.getElementById('legend-overlay').style.display = 'none';
}

function showTable() {
    // TODO: FIX THIS!!! This is currently hardcoded
    const myHeaders = new Headers();
    myHeaders.append("Content-Type", "application/json");

    // const raw = JSON.stringify({
    // "filePath": "/home/rjairaj/projects/Floating-Point-Histogram/UI/data/buffer3D3x8.bin",
    // "trieType": 3
    // });

    const requestOptions = {
        method: "GET",
        headers: myHeaders,
        // body: raw,
        redirect: "follow"
    };

    fetch("/process/", requestOptions)
        .then(response => response.json())
        .then(data => {
            let flatData = [];
            Object.keys(data).forEach(Z => {
                data[Z].forEach((row, X) => {
                    row.forEach((Count, Y) => {
                        if (Count > 0) {
                            flatData.push({ Z, X, Y, Count });
                        }
                    });
                });
            });

            const top10Data = flatData.sort((a, b) => b.Count - a.Count).slice(0, 10);
            const totalTop10Count = top10Data.reduce((sum, item) => sum + item.Count, 0);

            let overlay = document.querySelector("#overlay");
            if (!overlay) {
                overlay = document.createElement("div");
                overlay.setAttribute("id", "overlay");
                document.body.appendChild(overlay);
            }

            overlay.style.display = "block";
            overlay.style.position = "fixed";
            overlay.style.width = "80%";
            overlay.style.height = "80%";
            overlay.style.top = "10%";
            overlay.style.left = "10%";
            overlay.style.backgroundColor = "rgba(255, 255, 255, 1)";
            overlay.style.padding = "20px";
            overlay.style.border = "1px solid #ccc";
            overlay.style.borderRadius = "5px";
            overlay.style.zIndex = "1000";
            overlay.innerHTML = ''; // Clear previous content

            const closeButton = document.createElement("button");
            closeButton.textContent = "X";
            closeButton.onclick = function () { overlay.style.display = "none"; };
            overlay.appendChild(closeButton);

            const heading = document.createElement("h2");
            heading.textContent = "Top 10 TLE Bits";
            overlay.appendChild(heading);

            const table = document.createElement("table");
            table.style.width = "100%";
            overlay.appendChild(table);

            const thead = document.createElement("thead");
            table.appendChild(thead);
            const headerRow = document.createElement("tr");
            thead.appendChild(headerRow);
            ["X", "Y", "Z", "Count", "% of Total"].forEach(headerText => {
                const headerCell = document.createElement("th");
                headerCell.textContent = headerText;
                headerRow.appendChild(headerCell);
            });

            const tbody = document.createElement("tbody");
            table.appendChild(tbody);
            top10Data.forEach(item => {
                const row = document.createElement("tr");
                tbody.appendChild(row);
                ["X", "Y", "Z", "Count"].forEach(key => {
                    const cell = document.createElement("td");
                    // Check if the current key is 'Z' and adjust its display
                    if (key === "Z") {
                        // Assuming the Z value is stored as 'Z6', 'Z7', etc., extract the numeric part
                        const zValue = item[key].substring(1); // Remove the first character ('Z') to display only the number
                        cell.textContent = zValue;
                    } else {
                        cell.textContent = item[key];
                    }
                    row.appendChild(cell);
                });
                // Calculate and append the percentage cell
                const percentCell = document.createElement("td");
                percentCell.textContent = ((item.Count / totalTop10Count) * 100).toFixed(2) + "%";
                row.appendChild(percentCell);
            });

        })
        .catch(error => console.error("Error loading or processing linear.json:", error));
}

let currentScaleType = 'linear'; // Default scale type
let useLogScale = false;

function setAndLoadPlots(scaleType) {
    currentScaleType = scaleType;
    loadPlots(); // Now, loadPlots does not take any parameters
}

const loadPlots = async () => {
    const myHeaders = new Headers();
    myHeaders.append("Content-Type", "application/json");

    const requestOptions = {
        method: "GET",
        headers: myHeaders,
        redirect: "follow"
    };

    document.querySelector('.show-legend-button2').style.display = 'none'; // Do not Show the legend2 button

    const data = await makeApiRequest("/process/", requestOptions);
    const plotContainer = document.getElementById('plot-container');
    plotContainer.innerHTML = ''; // Clear existing plots

    // Flatten all matrix data into a single array and filter out zero values
    let allValues = Object.values(data).flat(Infinity).filter(value => value > 0);

    // Calculate global maximum and minimum non-zero values across all matrices
    let globalMaxValue = Math.max(...allValues);
    let globalMinValue = Math.min(...allValues);

    // Adjust for log scale if needed
    if (currentScaleType === 'log') {
        globalMaxValue = Math.log(globalMaxValue + 1); // Adding 1 to avoid log(0)
        globalMinValue = Math.log(globalMinValue + 1); // Adjust minimum for log scale too
    }

    // drawHeatmap(data, '1', '1', globalMinValue, globalMaxValue);

    // Object.keys(data).forEach((z, index) => {
    const matrixData = data;
    const wrapperDiv = document.createElement('div'); // Create a wrapper div
    wrapperDiv.classList.add('plot-wrapper');
    // const zLabelDiv = document.createElement('div'); // Create a div for the label
    // zLabelDiv.innerHTML = `Z = ${Labels[index]}`; // Use Labels array for labeling
    // zLabelDiv.classList.add('z-label');

    const plotId = `plot-${1}`;
    const plotDiv = document.createElement('div');
    plotDiv.id = plotId;
    plotDiv.classList.add('plot');

    // wrapperDiv.appendChild(zLabelDiv); // Append label div to wrapper
    wrapperDiv.appendChild(plotDiv); // Append plot div to wrapper
    plotContainer.appendChild(wrapperDiv); // Append wrapper to the plot container
    console.log("matrix Data");
    console.log(matrixData);
    // Pass the globalMaxValue and globalMinValue to drawHeatmap
    drawHeatmap(matrixData, plotId, globalMinValue, globalMaxValue);
    // });

}


function drawHeatmap(matrixData, containerId, globalMinValue, globalMaxValue) {
    const margin = { top: 50, right: 30, bottom: 30, left: 60 }, // Increased top and left margins for labels
        width = 700 - margin.left - margin.right,
        height = 700 - margin.top - margin.bottom,
        gridSize = width / 8;

    const svg = d3.select(`#${containerId}`)
        .append("svg")
        .attr("width", width + margin.left + margin.right)
        .attr("height", height + margin.top + margin.bottom)
        .append("g")
        .attr("transform", `translate(${margin.left},${margin.top})`);

    // Adding X and Y labels
    // Note: Adjust the positions as needed based on your layout
    LabelsSVG.forEach((label, i) => {
        // X labels
        const textElementX = svg.append("text")
            .attr("x", i * gridSize + gridSize / 2 + 15)
            .attr("y", -10) // Adjusted for bottom positioning
            .attr("text-anchor", "middle")
            .style("font-size", label.text.includes("∞") ? "24px" : "20px") // Increase font size for infinity symbol
            .attr("transform", `rotate(-90, ${i * gridSize + gridSize / 2}, -10)`);

        // Append the main text part for X labels
        textElementX.append("tspan").text(label.text);

        // Append superscript if present, adjusting positioning and size
        if (label.superscript) {
            textElementX.append("tspan")
                .attr("dx", "0.3em") // Slightly shift right from the main text
                .attr("dy", "-0.6em") // Shift up to simulate superscript
                .style("font-size", "10px") // Smaller font size for superscript
                .text(label.superscript);
        }

        // Y labels
        const textElementY = svg.append("text")
            .attr("x", -10) // Adjusted for left positioning
            .attr("y", i * gridSize + gridSize / 2)
            .attr("dy", ".35em") // Vertical alignment
            .attr("text-anchor", "end")
            .style("font-size", label.text.includes("∞") ? "24px" : "20px"); // Increase font size for infinity symbol

        // Append the main text part for Y labels
        textElementY.append("tspan").text(label.text);

        // Append superscript if present, adjusting positioning and size
        if (label.superscript) {
            textElementY.append("tspan")
                .attr("dx", "0.3em") // Slightly shift right from the main text
                .attr("dy", "-0.6em") // Shift up to simulate superscript
                .style("font-size", "10px") // Smaller font size for superscript
                .text(label.superscript);
        }
    });

    const tooltip = d3.select("#tooltip");
    let maxValue = globalMaxValue;
    const minGrayScale = 240;
    Object.keys(matrixData).forEach((row, rowIndex) => {
        Object.keys(matrixData[row]).forEach((col, colIndex) => {
            let value = matrixData[row][col];
            let grayscale;
            if (value === 0) {
                grayscale = 255; // White for zero values
            } else if (currentScaleType === 'linear') {
                // Ensure there's a minimum grayscale for any non-zero value
                // Set a minimum grayscale value for the smallest non-zero values
                const maxGrayScale = 0; // Maximum grayscale for the largest value

                if (globalMaxValue === globalMinValue) { // Handle case where all values are the same
                    grayscale = minGrayScale;
                } else {
                    const normalizedValue = (value - globalMinValue) / (globalMaxValue - globalMinValue);
                    // Interpolate between minGrayScale and maxGrayScale based on normalizedValue
                    grayscale = Math.round(minGrayScale + (normalizedValue * (maxGrayScale - minGrayScale)));
                }// Darker grayscale for higher values
            } else {
                // For log scale, normalize the log-transformed values
                const adjustedValue = currentScaleType === 'log' ? Math.log(value + 1) : value;
                // Normalize value to 0-255 scale for grayscale
                const normalizedValue = Math.floor((adjustedValue / maxValue) * 255);
                // Invert grayscale to have higher values darker
                grayscale = 255 - normalizedValue;
            }

            svg.append("rect")
                .attr("x", colIndex * gridSize)
                .attr("y", rowIndex * gridSize)
                .attr("width", gridSize)
                .attr("height", gridSize)
                .style("fill", `rgb(${grayscale},${grayscale},${grayscale})`)
                .style("stroke", "blue")
                .style("stroke-width", 1)
                .on("mouseover", function (event) {
                    tooltip.html(`Count: ${value.toFixed(0)}`)
                        .style("left", `${event.pageX + 10}px`)
                        .style("top", `${event.pageY + 10}px`)
                        .style("visibility", "visible");
                })
                .on("mouseout", function () {
                    tooltip.style("visibility", "hidden");
                })
                .on("click", function () {
                    console.log("Clicked X ", colIndex, " Y ", rowIndex);
                    displayMatrix(matrixData, colIndex, rowIndex);
                    document.querySelector('.show-arrow').style.display = 'block'; // Show the arrow
                });
        });
    });
}

function formatNumber(value) {
    // Check if the value is greater than 5 digits
    if (Math.abs(value) < 100000) {
        // Round the value to 5 significant digits without forcing trailing zeros
        return Number.parseFloat(value.toPrecision(5));
    } else {
        return value.toExponential(4);  // Show in scientific notation
    }
}

function showLegend2() {
    // Create the overlay div
    const overlay = d3.select('body').append('div')
    .attr('id', 'legend-overlay')
    .style('position', 'fixed')
    .style('top', '0')
    .style('left', '0')
    .style('width', '100%')
    .style('height', '100%')
    .style('background-color', 'transparent') // Fully transparent background
        .style('display', 'flex')
        .style('flex-direction', 'column')
        .style('align-items', 'center')
        .style('justify-content', 'center')
        .style('z-index', '1000') // High z-index to overlay everything
        .style('cursor', 'move') // Change cursor to indicate movability
        .call(dragOverlay); // Call the drag function to make the overlay movable

    // Add the content container
    const content = overlay.append('div')
    .attr('id', 'legend-content')
    .style('background-color', '#fff')
    .style('padding', '20px')
    .style('border-radius', '10px')
    .style('box-shadow', '0 4px 8px rgba(0, 0, 0, 0.3)') // Add shadow for better visibility
        .style('max-width', '80%')
        .style('max-height', '80%')
        .style('overflow', 'auto')
        .style('text-align', 'center');

    // Add title
    content.append('h2').text('Exponent Ranges');

    // Add the buttons
    const buttonContainer = content.append('div').attr('class', 'button-container');
    buttonContainer.append('button')
    .attr('id', 'positive-button')
    .style('padding', '10px 20px')
    .style('margin', '10px')
    .style('cursor', 'pointer')
    .text('Positive Exponent')
    .on('click', () => displayExponentData(true)); // True for positive exponents

    buttonContainer.append('button')
    .attr('id', 'negative-button')
    .style('padding', '10px 20px')
    .style('margin', '10px')
    .style('cursor', 'pointer')
    .text('Negative Exponent')
    .on('click', () => displayExponentData(false)); // False for negative exponents

    // Create a table for the results
    const table = content.append('table')
    .attr('id', 'exponent-table')
    .style('width', '100%')
    .style('border-collapse', 'collapse')
    .style('margin-top', '20px');

    // Add table headers
    const headerRow = table.append('thead').append('tr');
    headerRow.append('th').text('Index').style('border', '1px solid #ddd').style('padding', '8px');
    headerRow.append('th').text('Range').style('border', '1px solid #ddd').style('padding', '8px');

    // Default view: Positive Exponent data
    displayExponentData(true);

    // Add a close button
    const closeButton = content.append('button')
    .attr('id', 'close-button')
    .style('padding', '10px 20px')
    .style('margin-top', '20px')
    .style('cursor', 'pointer')
    .text('Close')
    .on('click', () => overlay.remove()); // Remove overlay when clicked

    // Drag behavior for the overlay
    function dragOverlay(selection) {
        let isDragging = false;
        let offsetX = 0, offsetY = 0;

        selection
        .on('mousedown', (event) => {
            isDragging = true;
            offsetX = event.clientX - overlay.node().getBoundingClientRect().left;
            offsetY = event.clientY - overlay.node().getBoundingClientRect().top;
        })
        .on('mousemove', (event) => {
            if (isDragging) {
                const xPos = event.clientX - offsetX;
                const yPos = event.clientY - offsetY;
                overlay.style('left', `${xPos}px`).style('top', `${yPos}px`);
            }
        })
        .on('mouseup', () => {
            isDragging = false;
        });
    }

    // Function to display exponent data
    function displayExponentData(isPositive) {
        // Clear the table body
        table.selectAll('tbody').remove();

        // Get the correct dataset (positive or negative)
        const data = isPositive ? positiveExponentData : negativeExponentData;

        // Calculate min/max for each Org_Index (0 to 15)
        const orgIndexRanges = [];
        for (let i = 0; i <= 15; i++) {
            const values = data
            .filter(row => row.Org_Index == i) // Filter rows for this Org_Index
                .map(row => parseFloat(row.approx_value)); // Extract the approx_value as numbers

            if (values.length > 0) {
                const minValue = Math.min(...values);
                const maxValue = Math.max(...values);
                orgIndexRanges.push({ orgIndex: i, range: formatNumber(minValue) + ' - ' + formatNumber(maxValue) });
            } else {
                orgIndexRanges.push({ orgIndex: i, range: 'N/A' });
            }
        }

        // Create table rows for the range values
        const tbody = table.append('tbody');
        orgIndexRanges.forEach(range => {
            const row = tbody.append('tr');
            row.append('td').text(range.orgIndex).style('border', '1px solid #ddd').style('padding', '8px');
            row.append('td').text(range.range).style('border', '1px solid #ddd').style('padding', '8px');
        });
    }

}




function visualizeGridData(gridData, useLogScale, filteredData, TLE_X, TLE_Y) {
    const container = d3.select('#plot-container2');
    container.html(''); // Clear existing content
    const tooltip = d3.select("#tooltip"); // Assuming tooltip div exists

    document.querySelector('.show-legend-button').style.display = 'flex'; // Show the legend button




    let allValues = Object.values(gridData).flat(Infinity);

    // Calculate global maximum and minimum non-zero values across all matrices
    let gridMaxValue = Math.max(...allValues);
    let gridMinValue = Math.min(...allValues);

    // Adjust for log scale if needed
    if (currentScaleType === 'log') {
        gridMaxValue = Math.log(gridMaxValue + 1); // Adding 1 to avoid log(0)
        gridMinValue = Math.log(gridMinValue + 1); // Adjust minimum for log scale too
    }

    const wrapperDiv = document.createElement('div');
    wrapperDiv.className = 'plot-wrapper2';
    container.node().appendChild(wrapperDiv);

    const margin = { top: 50, right: 20, bottom: 50, left: 60 },
        width = 900 - margin.left - margin.right,
        height = 900 - margin.top - margin.bottom;

    const svg = d3.select(wrapperDiv).append('svg')
    .attr('width', width + margin.left + margin.right)
    .attr('height', height + margin.top + margin.bottom)
    .style('border', '0px solid black')
    .append('g')
    .attr('transform', `translate(${margin.left},${margin.top})`);

    const minGrayScale = 240;

    // Helper function to get min/max approx_value from the CSV for the given xIndex/yIndex
    function getMinMaxForIndex(index, isPositiveMap) {
        let relevantData;
        if (isPositiveMap) {
            // Use the positive exponent map
            relevantData = positiveExponentData.filter(row => row.Org_Index == index);
        } else {
            // Use the negative exponent map
            relevantData = negativeExponentData.filter(row => row.Org_Index == index);
        }

        const approxValues = relevantData.map(row => parseFloat(row.approx_value));
        const minValue = Math.min(...approxValues);
        const maxValue = Math.max(...approxValues);

        return { minValue, maxValue };
    }

    // Update grid size references to 32x32
    gridData.forEach((row, yIndex) => {
        row.forEach((count, xIndex) => {
            let grayscale;
            if (count === 0) {
                grayscale = 255; // White for zero values
            } else if (currentScaleType === 'linear') {
                const maxGrayScale = 0; // Maximum grayscale for the largest value

                if (gridMaxValue === gridMinValue) { // Handle case where all values are the same
                    grayscale = minGrayScale;
                } else {
                    const normalizedValue = (count - gridMinValue) / (gridMaxValue - gridMinValue);
                    grayscale = Math.round(minGrayScale + (normalizedValue * (maxGrayScale - minGrayScale)));
                }
            } else {
                const logMin = gridMinValue > 0 ? Math.log(gridMinValue + 1) : 0; // Avoid log(0)
                const logMax = Math.log(gridMaxValue + 1);
                const logValue = Math.log(count + 1);
                const logRange = logMax - logMin;

                if (logRange === 0) { // Handle case where all log values are the same
                    grayscale = 255 - minGrayScale;
                } else {
                    const normalizedLogValue = (logValue - logMin) / logRange;
                    grayscale = Math.round((1 - normalizedLogValue) * (255 - minGrayScale) + minGrayScale);
                }
            }

            // Get the min and max approx_value for this xIndex and yIndex based on the mapping
            const xMinMax = getMinMaxForIndex(xIndex, TLE_X === 2 || TLE_X === 6);
            const yMinMax = getMinMaxForIndex(yIndex, TLE_Y === 2 || TLE_Y === 6);

            // Adjust rectangle sizes for 32x32 grid
            svg.append('rect')
            .attr('x', xIndex * (width / 16))
            .attr('y', yIndex * (height / 16))
            .attr('width', width / 16)
            .attr('height', height / 16)
            .attr('fill', `rgb(${grayscale}, ${grayscale}, ${grayscale})`)
            .on('mouseover', function () {
                tooltip.style("visibility", "visible").html(
                    `Count: ${count}<br>
                     Abs-Approx X: ${formatNumber(xMinMax.minValue)} - ${formatNumber(xMinMax.maxValue)}<br>
                     Abs-Approx Y: ${formatNumber(yMinMax.minValue)} - ${formatNumber(yMinMax.maxValue)}`
                );
            })
            .on('mousemove', function (event) {
                tooltip.style("top", (event.pageY - 10) + "px").style("left", (event.pageX + 10) + "px");
            })
            .on('mouseout', function () { tooltip.style("visibility", "hidden"); })
            .on('click', function () {
                const gridDataL2 = prepareGridDataL2(filteredData, xIndex, yIndex, count,TLE_X, TLE_Y);
                // Add a "Drill more" button to handle step-4

                if (currentScaleType == 'linear') {
                    visualizeGridDataL2(gridDataL2, useLogScale = false, xIndex, yIndex, count,TLE_X, TLE_Y,filteredData);
                }
                else if (currentScaleType == 'log') {
                    visualizeGridDataL2(gridDataL2, useLogScale = true, xIndex, yIndex, count,TLE_X, TLE_Y);
                }
                document.querySelector('.leveldisplay3').style.display = 'block';
                document.querySelector('.show-arrow3').style.display = 'block';

            });
        });
    });

    // Update labels and grid lines to match 32x32
    const labels = Array.from({ length: 16 }, (_, i) => i);
    labels.forEach((label, i) => {
        svg.append('text')
        .text(label)
        .attr('x', i * (width / 16) + (width / 32))
        .attr('y', 5)
        .attr('dy', '-1em')
        .style('text-anchor', 'middle')
        .style('font-size', '14px');

        svg.append('text')
        .text(label)
        .attr('x', 15)
        .attr('y', i * (height / 16) + (height / 32) + 5)
        .attr('dx', '-2em')
        .style('text-anchor', 'end')
        .style('font-size', '14px');
    });

    // Update grid line drawing for 32x32 grid
    for (let i = 0; i <= 16; i++) {
        svg.append('line')
        .attr('x1', 0).attr('y1', i * (height / 16))
        .attr('x2', width).attr('y2', i * (height / 16))
        .style('stroke', 'blue').style('stroke-width', '1');
        svg.append('line')
        .attr('x1', i * (width / 16)).attr('y1', 0)
        .attr('x2', i * (width / 16)).attr('y2', height)
        .style('stroke', 'blue').style('stroke-width', '1');
    }
}

function showLegend3(gridData, filteredData, orgxIndex, orgyIndex, orgCount, TLE_X, TLE_Y) {
    console.log("showLegend3 called with:");
    console.log("orgxIndex:", orgxIndex);
    console.log("orgyIndex:", orgyIndex);
    console.log("orgCount:", orgCount);
    console.log("TLE_X:", TLE_X);
    console.log("TLE_Y:", TLE_Y);
    console.log(filteredData);

    // Create the overlay div with fully transparent background
    const overlay = d3.select('body').append('div')
    .attr('id', 'legend-overlay')
    .style('position', 'fixed')
    .style('top', '0')
    .style('left', '0')
    .style('width', '100%')
    .style('height', '100%')
    .style('background-color', 'transparent') // Fully transparent background
        .style('display', 'flex')
        .style('flex-direction', 'column')
        .style('align-items', 'center')
        .style('justify-content', 'center')
        .style('z-index', '1000') // High z-index to overlay everything
        .style('cursor', 'move') // Optional: Movable overlay
        .call(dragOverlay); // Call the drag function to make the overlay draggable

    // Add the content container with a white background
    const content = overlay.append('div')
    .attr('id', 'legend-content')
    .style('background-color', '#fff') // White background for content
        .style('padding', '20px')
        .style('border-radius', '10px')
        .style('box-shadow', '0 4px 8px rgba(0, 0, 0, 0.3)') // Add shadow for better visibility
        .style('max-width', '80%')
        .style('max-height', '80%')
        .style('overflow', 'auto')
        .style('text-align', 'center')
        .style('position', 'relative'); // Make the content relative to the overlay

    // Add title
    content.append('h2').text('Approximate X, Y Values ');

    // Create a table for the results
    const table = content.append('table')
    .attr('id', 'exponent-table')
    .style('width', '100%')
    .style('border-collapse', 'collapse')
    .style('margin-top', '20px');

    // Add table headers
    const headerRow = table.append('thead').append('tr');
    headerRow.append('th').text('Index').style('border', '1px solid #ddd').style('padding', '8px');
    headerRow.append('th').text('Approx X').style('border', '1px solid #ddd').style('padding', '8px');
    headerRow.append('th').text('Approx Y').style('border', '1px solid #ddd').style('padding', '8px');

    // Helper function to get approximate values and exact count for a given index
    function getApproxAndCount(orgIndex, second_half_index, isPositive) {
        const data = isPositive ? positiveExponentData : negativeExponentData;

        // Find matching row for the given Org_Index and second_half_index
        const matchingRow = data.find(row => row.Org_Index == orgIndex && row.second_half_index == second_half_index);

        if (matchingRow) {
            return {
                approx_value: matchingRow.approx_value,
            };
        } else {
            return { approx_value: 'N/A'};
        }
    }

    // Create table rows for Org_Index with Approx X, Approx Y, and Exact Count for each second_half_index
    const tbody = table.append('tbody');
    for (let second_half_index = 0; second_half_index <= 15; second_half_index++) {
        const approxXData = getApproxAndCount(orgxIndex, second_half_index, TLE_X === 2 || TLE_X === 6);
        const approxYData = getApproxAndCount(orgyIndex, second_half_index, TLE_Y === 2 || TLE_Y === 6);

        const row = tbody.append('tr');
        row.append('td').text(second_half_index).style('border', '1px solid #ddd').style('padding', '8px');
        row.append('td').text(formatNumber(approxXData.approx_value)).style('border', '1px solid #ddd').style('padding', '8px');
        row.append('td').text(formatNumber(approxYData.approx_value)).style('border', '1px solid #ddd').style('padding', '8px');
    }

    // Add a close button
    const closeButton = content.append('button')
    .attr('id', 'close-button')
    .style('padding', '10px 20px')
    .style('margin-top', '20px')
    .style('cursor', 'pointer')
    .text('Close')
    .on('click', () => overlay.remove()); // Remove overlay when clicked

    // Helper function to format numbers
    function formatNumber(value) {
        if (typeof value === 'number') {
            return value < 100000 ? value.toFixed(5) : value.toExponential(3); // Format large numbers in scientific notation
        }
        return value; // Return 'N/A' or other string values as they are
    }

    // Drag behavior for the overlay
    function dragOverlay(selection) {
        let isDragging = false;
        let offsetX = 0, offsetY = 0;

        selection
        .on('mousedown', (event) => {
            isDragging = true;
            offsetX = event.clientX - overlay.node().getBoundingClientRect().left;
            offsetY = event.clientY - overlay.node().getBoundingClientRect().top;
        })
        .on('mousemove', (event) => {
            if (isDragging) {
                const xPos = event.clientX - offsetX;
                const yPos = event.clientY - offsetY;
                overlay.style('left', `${xPos}px`).style('top', `${yPos}px`);
            }
        })
        .on('mouseup', () => {
            isDragging = false;
        });
    }
}






function visualizeGridDataL2(gridData, useLogScale, orgxIndex, orgyIndex, orgCount,TLE_X, TLE_Y,filteredData) {
    const container = d3.select('#plot-container3');
    container.html(''); // Clear existing content
    const tooltip = d3.select("#tooltip2"); // Assuming tooltip div exists

    let allValues = Object.values(gridData).flat(Infinity);
    document.querySelector('.show-legend-button2').style.display = 'flex'; // Show the legend button
    const getshowLegendBtn = document.querySelector('.show-legend-button_btn');
    getshowLegendBtn.style.padding = "10px 20px";
    getshowLegendBtn.style.cursor = "pointer";
    getshowLegendBtn.textContent = 'Show Legend →';
    getshowLegendBtn.onclick = () => {
        showLegend3(gridData,filteredData,orgxIndex,orgyIndex,orgCount,TLE_X,TLE_Y);
    }
    // Calculate global maximum and minimum non-zero values across all matrices
    let gridMaxValue = Math.max(...allValues);
    let gridMinValue = Math.min(...allValues);

    console.log("gridMaxValue: ", gridMaxValue);
    console.log("gridMinValue: ", gridMinValue);

    // Adjust for log scale if needed
    if (currentScaleType === 'log') {
        gridMaxValue = Math.log(gridMaxValue + 1); // Adding 1 to avoid log(0)
        gridMinValue = Math.log(gridMinValue + 1); // Adjust minimum for log scale too
    }

    const wrapperDiv = document.createElement('div');
    wrapperDiv.className = 'plot-wrapper3';
    container.node().appendChild(wrapperDiv);

    const margin = { top: 50, right: 20, bottom: 50, left: 60 },
        width = 900 - margin.left - margin.right,
        height = 900 - margin.top - margin.bottom;

    const svg = d3.select(wrapperDiv).append('svg')
    .attr('width', width + margin.left + margin.right)
    .attr('height', height + margin.top + margin.bottom)
    .style('border', '0px solid black')
    .append('g')
    .attr('transform', `translate(${margin.left},${margin.top})`);

    const minGrayScale = 240;

    // Helper function to extract value for X or Y based on Org_Index and second_half_index
    function extractValueForAxis(isPositiveMap, orgIndex, halfIndex) {
        let relevantData;

        if (isPositiveMap) {
            relevantData = positiveExponentData;
        } else {
            relevantData = negativeExponentData;
        }

        // Find the row matching orgIndex and second_half_index
        const matchingRow = relevantData.find(row =>
            row.Org_Index == orgIndex && row.second_half_index == halfIndex);

        return matchingRow ? matchingRow.approx_value : null;
    }

    // Update grid size references to 32x32
    gridData.forEach((row, yIndex) => {
        row.forEach((count, xIndex) => {
            let grayscale;
            if (count === 0) {
                grayscale = 255; // White for zero values
            } else if (currentScaleType === 'linear') {
                const maxGrayScale = 0; // Maximum grayscale for the largest value

                if (gridMaxValue === gridMinValue) { // Handle case where all values are the same
                    grayscale = minGrayScale;
                } else {
                    const normalizedValue = (count - gridMinValue) / (gridMaxValue - gridMinValue);
                    grayscale = Math.round(minGrayScale + (normalizedValue * (maxGrayScale - minGrayScale)));
                }
            } else {
                const logMin = gridMinValue > 0 ? Math.log(gridMinValue + 1) : 0; // Avoid log(0)
                const logMax = Math.log(gridMaxValue + 1);
                const logValue = Math.log(count + 1);
                const logRange = logMax - logMin;

                if (logRange === 0) { // Handle case where all log values are the same
                    grayscale = 255 - minGrayScale;
                } else {
                    const normalizedLogValue = (logValue - logMin) / logRange;
                    grayscale = Math.round((1 - normalizedLogValue) * (255 - minGrayScale) + minGrayScale);
                }
            }

            // Extract the values for xIndex and yIndex independently
            const xValue = extractValueForAxis(TLE_X === 2 || TLE_X === 6, orgxIndex, xIndex);
            const yValue = extractValueForAxis(TLE_Y === 2 || TLE_Y === 6, orgyIndex, yIndex);

            // Adjust rectangle sizes for 32x32 grid
            svg.append('rect')
            .attr('x', xIndex * (width / 16))
            .attr('y', yIndex * (height / 16))
            .attr('width', width / 16)
            .attr('height', height / 16)
            .attr('fill', `rgb(${grayscale}, ${grayscale}, ${grayscale})`)
            .on('mouseover', function () {
                tooltip.style("visibility", "visible").html(
                    `Count: ${count}<br>Approx X Value: ${xValue || 'N/A'}<br>Approx Y Value: ${yValue || 'N/A'}`
                );
            })
            .on('mousemove', function (event) {
                tooltip.style("top", (event.pageY - 10) + "px").style("left", (event.pageX + 10) + "px");
            })
            .on('mouseout', function () {
                tooltip.style("visibility", "hidden");
            })
            .on('click', function () {
                document.querySelector('.show-arrow4').style.display = 'block'; // Show the arrow

                displayOriginalMPBitsForCell(yIndex, xIndex, count, orgxIndex, orgyIndex, orgCount,TLE_X, TLE_Y,filteredData);
            });
        });
    });

    // Add X and Y labels
    const labels = Array.from({ length: 16 }, (_, i) => i); // Generate labels 0-31
    labels.forEach((label, i) => {
        svg.append('text')
        .text(label)
        .attr('x', i * (width / 16) + (width / 32))
        .attr('y', 5)
        .attr('dy', '-1em')
        .style('text-anchor', 'middle')
        .style('font-size', '14px');

        svg.append('text')
        .text(label)
        .attr('x', 15)
        .attr('y', i * (height / 16) + (height / 32) + 5)
        .attr('dx', '-2em')
        .style('text-anchor', 'end')
        .style('font-size', '14px');
    });

    // Draw grid lines
    for (let i = 0; i <= 16; i++) {
        svg.append('line')
        .attr('x1', 0).attr('y1', i * (height / 16))
        .attr('x2', width).attr('y2', i * (height / 16))
        .style('stroke', 'blue').style('stroke-width', '1');
        svg.append('line')
        .attr('x1', i * (width / 16)).attr('y1', 0)
        .attr('x2', i * (width / 16)).attr('y2', height)
        .style('stroke', 'blue').style('stroke-width', '1');
    }
}

let gridDataL2 = Array(16).fill().map(() => Array(16).fill(0)); // Initialize 16x16 grid with zeros

function prepareGridDataL2(processedData, x, y,count,TLE_X, TLE_Y) {

    if (Object.keys(positiveExponentMap).length === 0) {
        console.error("Positive Exponent Map is not loaded yet.");
        return;
    }

    // Ensure that the negativeExponentMap is loaded before running this
    if (Object.keys(negativeExponentMap).length === 0) {
        console.error("Negative Exponent Map is not loaded yet.");
        return;
    }

    console.log("Inside Level 2 prepareGridData");
    console.log("TLE_X:", TLE_X);
    console.log("TLE_Y:", TLE_Y);
    console.log("x level 1:", x);
    console.log("y level 1:", y);

    processedData.forEach(item => {
        // Parse the M_X and the first bit of P_X, and M_Y and the first bit of P_Y as orgxIndex and orgyIndex
        let xIndex = parseInt(item.M_X,2); // Parse binary to decimal
        let yIndex = parseInt(item.M_Y,2); // Parse binary to decimal

        // Determine which mapping to use for xIndex based on TLE_X
        let orgXIndex;
        if (TLE_X === 2 || TLE_X === 6) {
            orgXIndex = mapPositiveIndexToOrgIndex(xIndex); // Use positive mapping
        } else if (TLE_X === 3 || TLE_X === 5) {
            orgXIndex = mapNegativeIndexToOrgIndex(xIndex); // Use negative mapping
        } else {
            console.error(`Unsupported TLE_X value: ${TLE_X}`);
            return;
        }

        // Determine which mapping to use for yIndex based on TLE_Y
        let orgYIndex;
        if (TLE_Y === 2 || TLE_Y === 6) {
            orgYIndex = mapPositiveIndexToOrgIndex(yIndex); // Use positive mapping
        } else if (TLE_Y === 3 || TLE_Y === 5) {
            orgYIndex = mapNegativeIndexToOrgIndex(yIndex); // Use negative mapping
        } else {
            console.error(`Unsupported TLE_Y value: ${TLE_Y}`);
            return;
        }

        // Check if the current item matches the target x and y
        if (orgXIndex === x && orgYIndex === y && count==item.Count_Level1) {
            console.log('orgxIndex:', orgXIndex, 'orgyIndex:', orgYIndex);
            console.log('Target x:', x, 'Target y:', y);

            // Extract only the last 5 bits of P_X and P_Y, excluding the first bit
            let l2xIndex = parseInt(item.P_X,2); // Take bits 1 to 5 (last 5 bits of P_X)
            let l2yIndex = parseInt(item.P_Y,2); // Take bits 1 to 5 (last 5 bits of P_Y)

            console.log('PREPARE GRID 2 Data - l2xIndex:', l2xIndex, 'l2yIndex:', l2yIndex);

            // Increment the grid based on the extracted indices
            gridDataL2[l2yIndex][l2xIndex] += item.Count;

            console.log('Updated gridDataL2:', gridDataL2);
        }
    });

    return gridDataL2;
}

function displayOriginalMPBitsForCell(clickedY, clickedX, count, xIndex, yIndex, orgCount,TLE_X,TLE_Y,filteredData) {
    console.log(`clickedX(Level2): ${clickedX}, clickedY(Level2): ${clickedY}, count: ${count}`);
    console.log(`orgCount: ${orgCount}`);

    console.log(`UnMapped x-index level1 : ${xIndex}, UnMapped x-index level2 : ${yIndex}`);
    console.log('Processing data:', processedData);
    if (Object.keys(positiveExponentMap).length === 0) {
        console.error("Positive Exponent Map is not loaded yet.");
        return;
    }

    // Ensure that the negativeExponentMap is loaded before running this
    if (Object.keys(negativeExponentMap).length === 0) {
        console.error("Negative Exponent Map is not loaded yet.");
        return;
    }


    // Logic to find the original M and P bits based on clicked cell's coordinates and count
    const matchingEntries = filteredData.filter(item => {
        // Extract M_X, M_Y, P_X, and P_Y from the data entry
        const m_x = item.M_X;
        const m_y = item.M_Y;
        const p_x = item.P_X;
        const p_y = item.P_Y;
        const countLevel1 = item.Count_Level1; // Assuming this is the orgCount

        // Extract the last 5 bits of P_X and P_Y
        const pX_last5Bits = p_x; // Last 5 bits of P_X
        const pY_last5Bits = p_y; // Last 5 bits of P_Y

        // Extract orgxIndex and orgyIndex (first bit of P_X + M_X, first bit of P_Y + M_Y)
        const orgXIndexData = parseInt(m_x,2); // M_X + first bit of P_X
        const orgYIndexData = parseInt(m_y,2);// M_Y + first bit of P_Y

        let orgxIndex;
        if (TLE_X == 2 || TLE_X == 6) {
            orgxIndex = mapPositiveIndexToOrgIndex(orgXIndexData);
        } else if (TLE_X == 3 || TLE_X == 5) {
            orgxIndex = mapNegativeIndexToOrgIndex(orgXIndexData);
        }

        let orgyIndex;
        if (TLE_Y == 2 || TLE_Y == 6) {
            orgyIndex = mapPositiveIndexToOrgIndex(orgYIndexData);
        } else if (TLE_Y == 3 || TLE_Y == 5) {
            orgyIndex = mapNegativeIndexToOrgIndex(orgYIndexData);
        }
        // Check if clickedX and clickedY match the last 5 bits of P_X and P_Y
        const clickedXMatch = clickedX === parseInt(pX_last5Bits, 2);
        const clickedYMatch = clickedY === parseInt(pY_last5Bits, 2);

        console.log("orgxIndex:", orgxIndex, "orgyIndex:", orgyIndex);
        // Check if orgxIndex and orgyIndex match the combined M_X + P_X[0] and M_Y + P_Y[0]
        const orgXMatch = orgxIndex === xIndex;
        const orgYMatch = orgyIndex === yIndex;

        // Verify if counts match
        const countMatch = count === item.Count && orgCount === countLevel1;
        // console.log("countMatch:", countMatch);
        if(orgXMatch){
            console.log("orgXMatch:", orgXMatch);
        }

        if(orgYMatch){
            console.log("orgYMatch:", orgYMatch);
        }

        // Return true if all conditions match
        return clickedXMatch && clickedYMatch && orgXMatch && orgYMatch && countMatch;
    });

    if (matchingEntries.length > 0) {
        // Handle multiple matching entries if needed
        matchingEntries.forEach(entry => {
            displayMPBits(entry); // Display M and P bits information
        });
    } else {
        console.error("No matching entries found for the clicked cell.");
    }
}


function displayMPBits_1D(entry, nonSpecialDim1, nonSpecialDim2, specialDims, specialValue) {
    console.log(entry);
    const xyzLabelsDiv = document.getElementById('xyz-labels');
    xyzLabelsDiv.innerHTML = '';
    const mBitsDiv = document.getElementById('m-bits');
    const pBitsDiv = document.getElementById('p-bits');

    // Clear existing content
    mBitsDiv.innerHTML = '<div class="mp-title">M bits</div>';
    pBitsDiv.innerHTML = '<div class="mp-title">P bits</div>';

    let dim1Name = nonSpecialDim1.dimension;
    console.log(dim1Name);
    [dim1Name].forEach((dim, index) => {
        // Determine the color based on the row (index)
        let colorClass = '';
        switch (index) {
            case 0: colorClass = 'bit-box2-blue'; break;
        }

        // Add XYZ labels
        const labelDiv = document.createElement('div');
        labelDiv.textContent = `${dim}:`;
        xyzLabelsDiv.appendChild(labelDiv);

        // Add M bits
        const mBitDiv = document.createElement('div');
        mBitDiv.appendChild(createBitDisplay2(entry[`M_${dim}`], colorClass));
        mBitsDiv.appendChild(mBitDiv);

        // Add P bits (first 5 bits or adjust as necessary)
        const pBitDiv = document.createElement('div');
        pBitDiv.appendChild(createBitDisplay2(entry[`P_${dim}`].substring(0, 5), colorClass));
        pBitsDiv.appendChild(pBitDiv);
    });
}

function displayMPBits(entry) {
    console.log(entry);
    const xyzLabelsDiv = document.getElementById('xyz-labels');
    xyzLabelsDiv.innerHTML = '';
    const mBitsDiv = document.getElementById('m-bits');
    const pBitsDiv = document.getElementById('p-bits');

    // Clear existing content
    mBitsDiv.innerHTML = '<div class="mp-title">M bits</div>';
    pBitsDiv.innerHTML = '<div class="mp-title">P bits</div>';

    ['X', 'Y'].forEach((dim, index) => {
        // Determine the color based on the row (index)
        let colorClass = '';
        switch (index) {
            case 0: colorClass = 'bit-box2-blue'; break;
            case 1: colorClass = 'bit-box2-green'; break;
        }

        // Add XYZ labels
        const labelDiv = document.createElement('div');
        labelDiv.textContent = `${dim}:`;
        xyzLabelsDiv.appendChild(labelDiv);

        // Add M bits
        const mBitDiv = document.createElement('div');
        mBitDiv.appendChild(createBitDisplay2(entry[`M_${dim}`], colorClass));
        mBitsDiv.appendChild(mBitDiv);

        // Add P bits (first 5 bits or adjust as necessary)
        const pBitDiv = document.createElement('div');
        pBitDiv.appendChild(createBitDisplay2(entry[`P_${dim}`].substring(0, 6), colorClass));
        pBitsDiv.appendChild(pBitDiv);
    });
}

function createBitDisplay2(bits, colorClass) {
    const container = document.createElement('div');
    bits.split('').forEach(bit => {
        const bitBox = document.createElement('div');
        bitBox.className = `bit-box2 ${colorClass}`; // Apply the color class
        bitBox.textContent = bit;
        container.appendChild(bitBox);
    });
    return container;
}

function isSpecialTLE(tleBit) {
    return [0, 1, 4, 7].includes(tleBit);
}


const makeApiRequest = async (endpoint, requestOptions) => {
    const response = await fetch(endpoint, requestOptions);
    const data = await response.json();
    return data;
}

const displayTLEBitsForCell = async (x, y) => {

    const myHeaders = new Headers();
    myHeaders.append("Content-Type", "application/json");

    const raw = JSON.stringify({
        "x": x,
        "y": y,
    });

    const requestOptions = {
        method: "POST",
        headers: myHeaders,
        body: raw,
        redirect: "follow"
    };

    const filteredData = await makeApiRequest(`/drill/`, requestOptions)
    processedData = filteredData;

    document.querySelector('.h3-text').style.display = 'block'; // Show the arrow

    if (filteredData.length > 0) {
        // Assuming there's only one item for each TLE_X, TLE_Y, TLE_Z combination
        const item = filteredData[0];
        const specialStatus = [item.TLE_X, item.TLE_Y].map(isSpecialTLE);

        const specialCount = specialStatus.filter(Boolean).length;

        const specialDimIndex = specialStatus.findIndex(status => status === true);
        const specialDimNames = [x, y];
        const specialDim = specialDimNames[specialDimIndex];

        const detailsView = document.getElementById('details-view-tle');
        //detailsView.innerHTML = ''; // Clear previous content
        const specialIndices = specialStatus
            .map((status, index) => status === true ? index : -1) // Map to indices or -1 if false
            .filter(index => index !== -1); // Filter out the -1 values

        const nonSpecialIndices = specialStatus
            .map((status, index) => status ? -1 : index)
            .filter(index => index !== -1);

        const nonSpecialTLEDetails = specialStatus
            .map((status, index) => {
                // if (!status) {
                // Return both the index and the value for non-special TLE bits
                return {
                    index,
                    value: [item.TLE_X, item.TLE_Y][index],
                    dimension: ['X', 'Y'][index] // Track dimension
                };
                // }
            })
            .filter(detail => detail !== null);


        const tleBitsX = item.TLE_X.toString(2).padStart(3, '0');
        const tleBitsY = item.TLE_Y.toString(2).padStart(3, '0');

        // Now create and display the TLE bits
        detailsView.appendChild(createBitDisplay('X', tleBitsX));
        detailsView.appendChild(createBitDisplay('Y', tleBitsY));


        // Add a "Drill more" button to handle step-4
        const drillMoreBtn = document.createElement('button');
        drillMoreBtn.className = 'drill-more-button';
        drillMoreBtn.style.padding = "10px 20px";
        drillMoreBtn.style.cursor = "pointer";
        drillMoreBtn.textContent = 'Drill more →';
        drillMoreBtn.onclick = () => {
            document.querySelector('.show-arrow2').style.display = 'block';
            if (specialCount == 0) {
                document.querySelector('.leveldisplay2').style.display = 'block';
                const preparedData = prepareGridData(filteredData,item.TLE_X,item.TLE_Y);
                console.log(preparedData);

                if (currentScaleType == 'linear') {
                    visualizeGridData(preparedData, useLogScale = false,filteredData,item.TLE_X,item.TLE_Y);
                }
                else if (currentScaleType == 'log') {
                    visualizeGridData(preparedData, useLogScale = true,filteredData,item.TLE_X,item.TLE_Y);
                }
            }
            else if (specialCount == 1) {
                specialDims = specialIndices.map(index => specialDimNames[index]);
                displayOriginalMPBitsForCell_1D(nonSpecialTLEDetails[0], specialStatus, specialDimNames[specialDimIndex], y, x);
            }
        };
        const showTable2Btn = document.createElement('button');
        showTable2Btn.className = 'show-table2-button';
        showTable2Btn.style.padding = "10px 20px";
        showTable2Btn.style.cursor = "pointer";
        showTable2Btn.textContent = 'Show Table';
        showTable2Btn.onclick = () => showTable2(filteredData);




        if (specialCount < 2) {
            detailsView.appendChild(showTable2Btn);

        }



        // Implement the showTable2 function
        const showTable2 = (filteredData) => {
            // Sort the filteredData by Count in descending order
            const sortedData = filteredData.sort((a, b) => b.Count - a.Count);
            // Pick the top 10 entries
            const top10Data = sortedData.slice(0, 10);
            // Calculate total count for percentage calculation
            const totalCount = top10Data.reduce((sum, item) => sum + item.Count, 0);

            // Create or find the overlay div for the table
            let overlay = document.getElementById("overlay-table2");
            if (!overlay) {
                overlay = document.createElement("div");
                overlay.id = "overlay-table2";
                document.body.appendChild(overlay);
            }
            overlay.innerHTML = ''; // Clear previous content
            overlay.style.display = 'block';
            // Styles for overlay
            overlay.style.position = 'fixed';
            overlay.style.width = '80%';
            overlay.style.height = 'auto';
            overlay.style.left = '10%';
            overlay.style.top = '10%';
            overlay.style.backgroundColor = 'white';
            overlay.style.zIndex = 1000;
            overlay.style.padding = '20px';
            overlay.style.border = '1px solid black';
            overlay.style.borderRadius = '5px';
            overlay.style.overflowY = 'auto';

            // Add a close button
            const closeButton = document.createElement('button');
            closeButton.textContent = 'X';
            closeButton.style.position = 'absolute';
            closeButton.style.right = '20px';
            closeButton.style.top = '20px';
            closeButton.onclick = () => overlay.style.display = 'none';
            overlay.appendChild(closeButton);

            // Add table heading
            const heading = document.createElement('h2');
            heading.textContent = 'Top 10 TLE Bits with M and P Bits';
            overlay.appendChild(heading);

            // Create the table
            const table = document.createElement('table');
            table.style.width = '100%';
            const thead = document.createElement('thead');
            const tbody = document.createElement('tbody');
            table.appendChild(thead);
            table.appendChild(tbody);
            overlay.appendChild(table);

            // Define table headers
            const headers = ['X', 'Y', 'Z', 'M Bits', 'P Bits', '% of Total'];
            const headerRow = document.createElement('tr');
            headers.forEach(header => {
                const th = document.createElement('th');
                th.textContent = header;
                headerRow.appendChild(th);
            });
            thead.appendChild(headerRow);

            // Populate table rows
            top10Data.forEach(item => {
                const row = document.createElement('tr');
                ['TLE_X', 'TLE_Y', 'TLE_Z', 'M Bits', 'P Bits'].forEach(key => {
                    const cell = document.createElement('td');
                    if (key === 'M Bits' || key === 'P Bits') {
                        // Concatenate M and P bits for display
                        const bits = key === 'M Bits' ? `X: ${item.M_X}, Y: ${item.M_Y}, Z: ${item.M_Z}` : `X: ${item.P_X}, Y: ${item.P_Y}, Z: ${item.P_Z}`;
                        cell.textContent = bits;
                    } else {
                        cell.textContent = item[key];
                    }
                    row.appendChild(cell);
                });
                // Calculate and display % of total
                const percentCell = document.createElement('td');
                percentCell.textContent = ((item.Count / totalCount) * 100).toFixed(2) + '%';
                row.appendChild(percentCell);
                tbody.appendChild(row);
            });
        }
        if (specialCount < 2) {
            detailsView.appendChild(drillMoreBtn);
        }
    } else {
        console.error('No data found for the clicked cell.');
    }
}
function displayOriginalMPBitsForCell_1D(nonSpecialDim1, specialDims, specialValue, y, x) {
    // Logic to find the original M and P bits based on clicked cell's coordinates and count

    document.querySelector('.show-arrow-outer-div').style.display = 'none'; // Show the arrow
    document.querySelector('.show-arrow-outer-div2').style.display = 'none'; // Show the arrow

    document.querySelector('.show-arrow-outer-div3').style.display = 'none'; // Show the arrow

    document.querySelector('.show-arrow4').style.display = 'block'; // Show the arrow


    
    const matchingEntries = processedData.filter(item =>
        item.TLE_X === x && item.TLE_Y === y
    );

    console.log(matchingEntries);
    if (matchingEntries.length > 0) {
        // Assuming there might be multiple matching entries, handle accordingly
        matchingEntries.forEach(entry => {
            displayMPBits_1D(entry, nonSpecialDim1, specialDims, specialValue); // Ensure this function is defined to display M and P bits information
        });
    } else {
        console.error("No matching entries found for the clicked cell.");
    }
}

function createBitDisplay(label, bits) {
    const outsideContainer = document.createElement('div'); // Create the outer container
    outsideContainer.className = 'outside-bit-container';

    const container = document.createElement('div'); // This remains your bit container
    container.className = 'bit-container';

    const labelDiv = document.createElement('span');
    labelDiv.textContent = label + ': ';
    container.appendChild(labelDiv);

    bits.split('').forEach(bit => {
        const bitBox = createBitBox(bit);
        container.appendChild(bitBox);
    });

    outsideContainer.appendChild(container); // Append the bit container to the outside container

    return outsideContainer; // Return the outside container instead
}


function displayMatrix(matrixData, clickedX, clickedY) {
    console.log("Display matrix");
    console.log("ClickedX: ", clickedX);
    console.log("ClickedY: ", clickedY);
    const detailsView = document.getElementById('details-view');
    detailsView.innerHTML = ''; // Clear existing content

    // Wrapper div for centering the matrix and labels
    const wrapperDiv = document.createElement('div');
    wrapperDiv.style.display = 'flex';
    wrapperDiv.style.flexDirection = 'column';
    wrapperDiv.style.alignItems = 'center'; // Center the content
    detailsView.appendChild(wrapperDiv);

    // const heading = document.createElement('p');
    // heading.className = "fst-Matrix-text";
    // heading.textContent = `Matrix for Z=${Labels_sup[intZ]} (clicked cell in yellow):`;
    // wrapperDiv.appendChild(heading);

    // Y Labels container (for alignment with matrix)
    const yLabelsContainer = document.createElement('div');
    yLabelsContainer.style.display = 'flex';
    yLabelsContainer.style.flexDirection = 'column';
    yLabelsContainer.style.alignItems = 'flex-end';

    // X Labels container
    const xLabelsContainer = document.createElement('div');
    xLabelsContainer.style.display = 'flex';
    xLabelsContainer.style.justifyContent = 'center';
    xLabelsContainer.style.width = '680px'; // Adjust based on your matrix size


    Labels_sup.forEach((label, i) => {
        const xLabel = document.createElement('div');
        xLabel.textContent = label;
        xLabel.style.width = '90px';
        xLabel.style.marginLeft = '50px';
        xLabel.style.textAlign = 'center';
        xLabelsContainer.appendChild(xLabel);
    });

    // Include X labels at the top
    wrapperDiv.appendChild(xLabelsContainer);

    // Layout container for Y labels and matrix
    const layoutContainer = document.createElement('div');
    layoutContainer.style.display = 'flex';
    layoutContainer.style.flexDirection = 'row';
    layoutContainer.style.alignItems = 'center'; // Align items in the center

    // Matrix cells container
    const cellsContainer = document.createElement('div');
    cellsContainer.style.display = 'flex';
    cellsContainer.style.flexDirection = 'column';

    Object.keys(matrixData).forEach((row, rowIndex) => {
        const rowContainer = document.createElement('div');
        rowContainer.style.display = 'flex';
        matrixData[row].forEach((col, colIndex) => {
            let value = col;
            const cell = document.createElement('div');
            cell.textContent = value.toFixed(0); // Adjust as needed
            cell.style.width = '80px';
            cell.style.height = '80px';
            cell.style.border = '1px solid #ddd';
            cell.style.textAlign = 'center';
            cell.style.lineHeight = '80px';
            cell.style.backgroundColor = (rowIndex === clickedY && colIndex === clickedX) ? 'yellow' : 'transparent';
            cell.addEventListener('click', () => {
                // Assuming you want to handle the click similar to how initial clicks were handled
                // You may need to adjust this to fit step-3 requirements
                console.log("Cell clicked: ", colIndex, rowIndex);
                displayTLEBitsForCell(colIndex, rowIndex);
            });
            rowContainer.appendChild(cell);
        });
        cellsContainer.appendChild(rowContainer);
    });

    // Add Y labels
    Labels_sup.forEach((label, i) => {
        const yLabel = document.createElement('div');
        yLabel.textContent = label;
        yLabel.style.height = '80px'; // Match cell height
        yLabel.style.marginRight = '20px';
        yLabel.style.display = 'flex';
        yLabel.style.alignItems = 'center'; // Center the text vertically
        yLabelsContainer.appendChild(yLabel);
    });

    // Assemble layout with Y labels before the matrix
    layoutContainer.appendChild(yLabelsContainer);
    layoutContainer.appendChild(cellsContainer);

    // Add layout container to the wrapper div, after X labels
    wrapperDiv.appendChild(layoutContainer);
}


function createBitBox(bit) {
    const bitBox = document.createElement('div');
    bitBox.textContent = bit;
    bitBox.className = 'bit-box'; // Apply CSS class for styling
    return bitBox;
}

// Call this function from within handleCellClick after calculating totalCount

let gridData = Array(16).fill().map(() => Array(16).fill(0)); // Initialize 16x16 grid with zeros

// You need to implement isSpecialTLE based on your criteria for what makes a TLE bit special
let orgToPositiveFirstHalfMap = {};
let orgToNegativeFirstHalfMap = {};
// Function to load and map the CSV file
function loadPositiveExponentDataGlobally() {
    d3.csv("../static/csv/positive_exponent_combinations.csv").then(data => {
        // Log the keys (column names) from the first row to check if they match
        positiveExponentData = data;
        console.log("CSV Column Names:", Object.keys(data[0]));

        // Create a mapping from first_half_index to Org_index
        data.forEach(row => {
            // console.log(`Mapping ${row.first_half_index} to ${row.Org_Index}`); // Debugging
            positiveExponentMap[row.first_half_index] = row.Org_Index;
            orgToPositiveFirstHalfMap[row.Org_Index] = row.first_half_index;
        });
        console.log("CSV data loaded and mapped globally.");
    }).catch(error => {
        console.error("Error loading the CSV file:", error);
    });
}

function loadNegativeExponentDataGlobally() {
    d3.csv("../static/csv/negative_exponent_combinations.csv").then(data => {
        // Log the keys (column names) from the first row to check if they match
        negativeExponentData = data;
        console.log("CSV Column Names (Negative Exponent):", Object.keys(data[0]));

        // Create a mapping from first_half_index to Org_index
        data.forEach(row => {
            // console.log(`Mapping ${row.first_half_index} to ${row.Org_Index}`); // Debugging
            negativeExponentMap[row.first_half_index] = row.Org_Index;
            orgToNegativeFirstHalfMap[row.Org_Index] = row.first_half_index;
        });
        console.log("Negative exponent CSV data loaded and mapped globally.");
    }).catch(error => {
        console.error("Error loading the negative_exponent_combinations.csv file:", error);
    });
}

function mapPositiveIndexToOrgIndex(index) {
    // Convert the index to string (as the map keys are strings)
    const orgIndex = positiveExponentMap[index];

    // Check if the index exists in the map and return the Org_Index
    if (orgIndex !== undefined) {
        return parseInt(orgIndex, 10); // Convert the Org_Index back to an integer
    } else {
        console.error(`Index ${index} not found in the CSV map.`);
        return null; // Return null if no mapping is found
    }
}

// Function to map xIndex or yIndex to Org_Index using the global negative exponent map
function mapNegativeIndexToOrgIndex(index) {
    // Convert the index to string (as the map keys are strings)
    const orgIndex = negativeExponentMap[index];

    // Check if the index exists in the map and return the Org_Index
    if (orgIndex !== undefined) {
        return parseInt(orgIndex, 10); // Convert the Org_Index back to an integer
    } else {
        console.error(`Index ${index} not found in the negative exponent CSV map.`);
        return null; // Return null if no mapping is found
    }
}

// Function to map Org_Index back to first_half_index for positive map
function mapOrgIndexToPositiveFirstHalf(orgIndex) {
    const firstHalfIndex = orgToPositiveFirstHalfMap[orgIndex];
    if (firstHalfIndex !== undefined) {
        return firstHalfIndex;
    } else {
        console.error(`Org_Index ${orgIndex} not found in the positive exponent reverse map.`);
        return null; // Return null if no mapping is found
    }
}

// Function to map Org_Index back to first_half_index for negative map
function mapOrgIndexToNegativeFirstHalf(orgIndex) {
    const firstHalfIndex = orgToNegativeFirstHalfMap[orgIndex];
    if (firstHalfIndex !== undefined) {
        return firstHalfIndex;
    } else {
        console.error(`Org_Index ${orgIndex} not found in the negative exponent reverse map.`);
        return null; // Return null if no mapping is found
    }
}



function prepareGridData(processedData, TLE_X, TLE_Y) {
    // Ensure that the positiveExponentMap is loaded before running this
    if (Object.keys(positiveExponentMap).length === 0) {
        console.error("Positive Exponent Map is not loaded yet.");
        return;
    }

    // Ensure that the negativeExponentMap is loaded before running this
    if (Object.keys(negativeExponentMap).length === 0) {
        console.error("Negative Exponent Map is not loaded yet.");
        return;
    }

    console.log("Inside Level 1 prepareGridData");
    console.log("TLE_X:", TLE_X);
    console.log("TLE_Y:", TLE_Y);
    // let tempData = processedData;

    // tempData = tempData.filter(item => (item.M_X + "1" === item.M_X + item.P_X[0]) || (item.M_Y + "1" === item.M_Y + item.P_Y[0]) || (item.M_X + "0" === item.M_X + item.P_X[0]) || (item.M_Y + "0" ===item.M_Y + item.P_Y[0]));

    // console.log(tempData.length);
    const updatedCells = new Set();
    console.log(processedData.length);
    processedData.forEach(item => {
        // Parse xIndex and yIndex by concatenating M_X with the first bit of P_X and M_Y with the first bit of P_Y

        if(item.count > 0){
            console.log("Count:",item.count);
        }
        let xIndex = parseInt(item.M_X,2);
        let yIndex = parseInt(item.M_Y,2);

        // Determine which mapping to use for xIndex based on TLE_X
        let orgXIndex;
        if (TLE_X === 2 || TLE_X === 6) {
            orgXIndex = mapPositiveIndexToOrgIndex(xIndex); // Use positive mapping
        } else if (TLE_X === 3 || TLE_X === 5) {
            orgXIndex = mapNegativeIndexToOrgIndex(xIndex); // Use negative mapping
        } else {
            console.error(`Unsupported TLE_X value: ${TLE_X}`);
            return;
        }

        // Determine which mapping to use for yIndex based on TLE_Y
        let orgYIndex;
        if (TLE_Y === 2 || TLE_Y === 6) {
            orgYIndex = mapPositiveIndexToOrgIndex(yIndex); // Use positive mapping
        } else if (TLE_Y === 3 || TLE_Y === 5) {
            orgYIndex = mapNegativeIndexToOrgIndex(yIndex); // Use negative mapping
        } else {
            console.error(`Unsupported TLE_Y value: ${TLE_Y}`);
            return;
        }

            // gridData[yIndex][xIndex] = item.Count_Level1;


        // Ensure valid indices before updating the grid
        if (orgXIndex !== null && orgYIndex !== null) {
            const cellKey = `${orgYIndex},${orgXIndex}`;

            // Check if this cell has already been updated
            if (!updatedCells.has(cellKey)) {
                // Update the grid data and mark the cell as updated
                gridData[orgYIndex][orgXIndex] = item.Count_Level1;
                updatedCells.add(cellKey); // Add the cell to the set to prevent duplicates
            } else {
                console.warn(`Duplicate entry skipped for cell: ${cellKey}`);
            }
        } else {
            console.error(`Invalid orgXIndex or orgYIndex for item:`, item);
        }
    });

    return gridData;
}

document.addEventListener('DOMContentLoaded', async () => {
    // loadProcessedData();
    await loadPlots('linear');
    loadPositiveExponentDataGlobally();
    loadNegativeExponentDataGlobally();
});
