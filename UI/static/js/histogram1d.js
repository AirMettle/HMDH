// Global variables
l1_data = {}
let positiveExponentSortedData = {}
let negativeExponentSortedData = {}
let posMapIndexToFirstHalf4MSB = []
let negMapIndexToFirstHalf4MSB = []
let posMapIndexToFirstHalf4LSB = []
let negMapIndexToFirstHalf4LSB = []
let posMapIndexToSecondHalf4MSB = []
let negMapIndexToSecondHalf4MSB = []
let posMapIndexToSecondHalf4LSB = []
let negMapIndexToSecondHalf4LSB = []

const labelToChar = {
    "0": "a", 
    "1": "b",
    "2": "c",
    "3": "d",
    "4": "e",
    "5": "f",
    "6": "g",
    "7": "h",
    "8": "-h",
    "9": "-g",
    "10": "-f",
    "11": "-e",
    "12": "-d",
    "13": "-c",
    "14": "-b",
    "15": "-a"
}
const plot_width = 1500;
const plot_height = 600;
const color_picker = {
    "8": "#b22222",
    "9": "#b22222",
    "10": "#b22222",
    "11": "#b22222",
    "12": "#8A2BE2",
    "13": "#8A2BE2",
    "14": "#8A2BE2",
    "15": "#8A2BE2",
    "0": "#1e90ff",
    "1": "#1e90ff",
    "2": "#1e90ff",
    "3": "#1e90ff",
    "4": "#2f4f4f",
    "5": "#2f4f4f",
    "6": "#2f4f4f",
    "7": "#2f4f4f",
    "NaN": "#696969",
    "-Inf": "#696969",
    "Zero": "#696969",
    "+Inf": "#696969"
};

const gradient_color_picker = {
    "8": ["#b03030", "#963535", "#733434"],
    "9": [],
    "10": [],
    "11": [],
    "12": [],
    "13": [],
    "14": [],
    "15": [],
    "0": [],
    "1": [],
    "2": [],
    "3": [],
    "4": [],
    "5": [],
    "6": [],
    "7": [],
    "NaN": [],
    "-Inf": [],
    "Zero": [],
    "+Inf": []
};

let parent_color_idx = -1;

function resetPage() {
    location.reload();
}

let currentScaleType = 'linear'; // Default scale type
let useLogScale = false;


function setAndLoadPlots(scaleType) {
    currentScaleType = scaleType;
    loadPlots();
}

const makeApiRequest = async (endpoint, requestOptions) => {
    const response = await fetch(endpoint, requestOptions);
    const data = await response.json();
    return data;
}

const loadPlots = async () => {
    const myHeaders = new Headers();
    myHeaders.append("Content-Type", "application/json");

    const requestOptions = {
        method: "GET",
        headers: myHeaders,
        redirect: 'follow'
    }

    const matrixData = await makeApiRequest("/process/", requestOptions)

    drawTLEplot(matrixData, currentScaleType);
}

function drawTLEplot(matrixData, scaleType) {
    // Extract the first object for special counts
    const specialCounts = matrixData[0];
    // Clear the content of the container
    document.getElementById("top-level-plot-container").innerHTML = "";

    // Extract data for the bar plot
    const data = matrixData[1][0];

    // Prepare data for the bar plot
    const plotData = [
        { label: 'NaN', alt_label: "NaN", count: specialCounts['NaN'], originalCount: specialCounts['NaN'] },
        { label: '', alt_label: "", count: 0, originalCount: 0 }, // Empty for a break
        { label: '-Inf', alt_label: "-Inf", count: specialCounts['-Inf'], originalCount: specialCounts['-Inf'] },
        { label: '8', alt_label: "-h", count: data[8], originalCount: data[8] },
        { label: '9', alt_label: "-g", count: data[9], originalCount: data[9] },
        { label: '10', alt_label: "-f", count: data[10], originalCount: data[10] },
        { label: '11', alt_label: "-e", count: data[11], originalCount: data[11] },
        { label: '12', alt_label: "-d", count: data[12], originalCount: data[12] },
        { label: '13', alt_label: "-c", count: data[13], originalCount: data[13] },
        { label: '14', alt_label: "-b", count: data[14], originalCount: data[14] },
        { label: '15', alt_label: "-a", count: data[15], originalCount: data[15] },
        { label: 'Zero', alt_label: "Zero", count: specialCounts['Zero'], originalCount: specialCounts['Zero'] },
        { label: '0', alt_label: "a", count: data[0], originalCount: data[0] },
        { label: '1', alt_label: "b", count: data[1], originalCount: data[1] },
        { label: '2', alt_label: "c", count: data[2], originalCount: data[2] },
        { label: '3', alt_label: "d", count: data[3], originalCount: data[3] },
        { label: '4', alt_label: "e", count: data[4], originalCount: data[4] },
        { label: '5', alt_label: "f", count: data[5], originalCount: data[5] },
        { label: '6', alt_label: "g", count: data[6], originalCount: data[6] },
        { label: '7', alt_label: "h", count: data[7], originalCount: data[7] },
        { label: '+Inf', alt_label: "+Inf", count: specialCounts['+Inf'], originalCount: specialCounts['+Inf'] }
    ];

    // Normalize counts for logarithmic scale
    if (scaleType === "log") {
        plotData.forEach(d => {
            d.count = d.count > 0 ? Math.log(d.count) : 0; // Normalize using log for bar height
        });
    }

    // Set dimensions and margins for the plot
    const margin = { top: 20, right: 30, bottom: 40, left: 100 };
    const width = plot_width - margin.left - margin.right;
    const height = plot_height - margin.top - margin.bottom;

    // Create the SVG container
    const svg = d3.select("#top-level-plot-container")
        .append("svg")
        .attr("width", width + margin.left + margin.right)
        .attr("height", height + margin.top + margin.bottom)
        .append("g")
        .attr("transform", `translate(${margin.left}, ${margin.top})`);

    // Set the X scale
    const x = d3.scaleBand()
        .domain(plotData.map(d => d.label))
        .range([0, width])
        .padding(0.1);

    // Set the Y scale based on the scaleType
    const y = scaleType === "log"
        ? d3.scaleLog()
            .domain([1, d3.max(plotData, d => d.count)]) // Use a minimum value of 1 for log scale
            .range([height, 0])
        : d3.scaleLinear()
            .domain([0, d3.max(plotData, d => d.count)])
            .nice()
            .range([height, 0]);

    // Append the X axis to the SVG
    svg.append("g")
        .attr("class", "x-axis")
        .attr("transform", `translate(0, ${height})`)
        // .call(d3.axisBottom(x))
        .call(d3.axisBottom(x).tickFormat((d) => {
            const item = plotData.find(item => item.label === d)
            // Find the corresponding data entry
            return item ? item.alt_label : d; // Use alt_label if available 
        }))
        .attr("font-size", "12px");

    // Append the Y axis to the SVG
    svg.append("g")
        .attr("class", "y-axis")
        .call(d3.axisLeft(y))
        .attr("font-size", "12px");

    // Create bars
    svg.selectAll(".bar")
        .data(plotData)
        .enter()
        .append("rect")
        .attr("class", "bar")
        .attr("x", d => x(d.label))
        .attr("y", d => y(d.count))
        .attr("width", x.bandwidth())
        .attr("height", d => height - y(d.count))
        .attr("fill", d => color_picker[d.label])
        .on("mouseover", function (event, d) {
            // Change color on hover
            d3.select(this).attr("fill", "darkblue");
            // Only show overlay for valid integers
            if (!isNaN(d.label) && d.label !== '' && d.label !== 'NaN' && d.label !== '-Inf' && d.label !== '+Inf') {
                // Prepare overlay text based on label
                let overlayText = '';
                const labelNum = parseInt(d.label, 10);
                if (labelNum >= 8 && labelNum <= 11) {
                    overlayText = `- Sign  +ve Exponent`; // Negative sign, +ve exponent
                } else if (labelNum >= 12 && labelNum <= 15) {
                    overlayText = `- Sign  -ve Exponent`; // Negative sign, -ve exponent
                } else if (labelNum >= 0 && labelNum <= 4) {
                    overlayText = `+ Sign  +ve Exponent`; // +ve sign, +ve exponent
                } else if (labelNum >= 4 && labelNum <= 7) {
                    overlayText = `+ Sign  -ve Exponent`; // +ve sign, -ve exponent
                }

                // Set tooltip position and content
                const tooltip = d3.select("#tooltip");
                tooltip.style("visibility", "visible")
                    .style("left", (event.pageX + 5) + "px")
                    .style("top", (event.pageY - 5) + "px")
                    .text(overlayText);
            }
        })
        .on("mousemove", function (event) {
            const tooltip = d3.select("#tooltip");
            tooltip.style("left", (event.pageX + 5) + "px")
                .style("top", (event.pageY - 5) + "px");
        })
        .on("mouseout", function (event, d) {
            // Revert color
            d3.select(this).attr("fill", color_picker[d.label]);
            // Hide tooltip
            d3.select("#tooltip").style("visibility", "hidden");
        })
        .on("click", function (event, d) {
            // Call another function with the clicked data
            parent_color_idx = d.label;
            document.getElementsByClassName('show-arrow')[0].style.display = 'flex';
            plotLevel0part2(d.label, d.originalCount, matrixData[1][1], scaleType); // Use original count on click
        });

    // Display the original count on top of the bars
    svg.selectAll(".count-label")
        .data(plotData)
        .enter()
        .append("text")
        .attr("class", "count-label")
        .attr("x", d => x(d.label) + x.bandwidth() / 2)
        .attr("y", d => y(d.count) - 5) // Position above the bar
        .attr("text-anchor", "middle")
        .text(d => d.originalCount > 0 ? d.originalCount.toLocaleString() : '') // Format count with commas
        .on("click", function (e, d) {
            parent_color_idx = d.label;
            document.getElementsByClassName('show-arrow')[0].style.display = 'flex';
            plotLevel0part2(d.label, d.originalCount, matrixData[1][1], scaleType); // Use original count on click
        })
        .on("mouseover", function (e, d) {
            d3.select(this).style("cursor", "pointer")
        })
        .on("mouseout", function (e, d) {
            d3.select(this).style("cursor", "default")
        });
}

const reset_containers = (container_id) => {
    if (container_id === "Level0-plot-container") {
        document.getElementById("Level0-plot-container").innerHTML = "";
        document.getElementById("Level1-plot-container").innerHTML = "";
        document.getElementById("Level1-plot-container2").innerHTML = "";
    } else if (container_id === "Level1-plot-container") {
        document.getElementById("Level1-plot-container").innerHTML = "";
        document.getElementById("Level1-plot-container2").innerHTML = "";
    } else {
        document.getElementById("Level1-plot-container2").innerHTML = "";
    }
};

const drawPlot16 = (container_id, l0_1_data, scaleType, msb_4_bits) => {

    // Clear the content of the container
    reset_containers(container_id);

    let lower_color_id = 0;
    if (container_id === "Level0-plot-container") {
        lower_color_id = 0;
    }

    // Extract data for the bar plot
    const data = l0_1_data;
    let plotData = [
        { label: '0', count: data[0], originalCount: data[0] },
        { label: '1', count: data[1], originalCount: data[1] },
        { label: '2', count: data[2], originalCount: data[2] },
        { label: '3', count: data[3], originalCount: data[3] },
        { label: '4', count: data[4], originalCount: data[4] },
        { label: '5', count: data[5], originalCount: data[5] },
        { label: '6', count: data[6], originalCount: data[6] },
        { label: '7', count: data[7], originalCount: data[7] },
        { label: '8', count: data[8], originalCount: data[8] },
        { label: '9', count: data[9], originalCount: data[9] },
        { label: '10', count: data[10], originalCount: data[10] },
        { label: '11', count: data[11], originalCount: data[11] },
        { label: '12', count: data[12], originalCount: data[12] },
        { label: '13', count: data[13], originalCount: data[13] },
        { label: '14', count: data[14], originalCount: data[14] },
        { label: '15', count: data[15], originalCount: data[15] },
    ];
    // Prepare data for the bar plot
    if (container_id == "Level0-plot-container") {
        if (posMapIndexToFirstHalf4LSB !== null) {
            plotData = posMapIndexToFirstHalf4LSB.map(posMapItem => plotData.find(plotItem => plotItem.label === posMapItem.label));
        } else if (negMapIndexToFirstHalf4LSB !== null) {
            plotData = negMapIndexToFirstHalf4LSB.map(negMapItem => plotData.find(plotItem => plotItem.label === negMapItem.label) );
        }
    }
    
    // Normalize counts for logarithmic scale
    if (scaleType === "log") {
        plotData.forEach(d => {
            d.count = d.originalCount > 0 ? Math.log(d.originalCount + 1e-6) : null; // Null for zero counts
        });
    }

    // Set dimensions and margins for the plot
    const margin = { top: 20, right: 30, bottom: 40, left: 100 };
    const width = plot_width - margin.left - margin.right;
    const height = plot_height - margin.top - margin.bottom;
    const minHeight = 30;

    // Create the SVG container
    const svg = d3.select(`#${container_id}`)
        .append("svg")
        .attr("width", width + margin.left + margin.right)
        .attr("height", height + margin.top + margin.bottom)
        .append("g")
        .attr("transform", `translate(${margin.left}, ${margin.top})`);

    // Set the X scale
    const x = d3.scaleBand()
        .domain(plotData.map(d => d.label))
        .range([0, width])
        .padding(0.3);

    // Set the Y scale based on the scaleType
    const y = scaleType === "log"
        ? d3.scaleLog()
            .domain([1, d3.max(plotData, d => d.count !== null ? d.count : 1)]) // Ignore null values
            .range([height, 0])
        : d3.scaleLinear()
            .domain([0, d3.max(plotData, d => d.count)])
            .nice()
            .range([height, 0]);

    // Append the X axis to the SVG
    svg.append("g")
        .attr("class", "x-axis")
        .attr("transform", `translate(0, ${height})`)
        .call(d3.axisBottom(x))
        .attr("font-size", "12px");

    // Append the Y axis to the SVG
    svg.append("g")
        .attr("class", "y-axis")
        .call(d3.axisLeft(y))
        .attr("font-size", "12px");

    // Create bars, filtering out entries with null `count` (for zero originalCounts)
    svg.selectAll(".bar")
        .data(plotData.filter(d => d.count !== null)) // Filter bars for non-zero originalCount
        .enter()
        .append("rect")
        .attr("class", "bar")
        .attr("x", d => x(d.label))
        .attr("y", d => {
            const barHeight = height - y(d.count);
            return scaleType === "log" && barHeight < minHeight ? y(d.count) - (minHeight - barHeight) : y(d.count);
        })
        .attr("width", x.bandwidth())
        .attr("height", d => {
            const barHeight = height - y(d.count);
            return scaleType === "log" && barHeight < minHeight ? minHeight : barHeight;
        })
        .attr("fill", color_picker[parent_color_idx])
        .on("mouseover", function (event, d) {
            // Change color on hover
            d3.select(this).attr("fill", "darkblue");
            // Only show overlay for valid integers
            if (!isNaN(d.label) && d.label !== '' && d.label !== 'NaN' && d.label !== '-Inf' && d.label !== '+Inf') {
                // Prepare overlay text based on label
                let overlayText = '';
                const labelNum = parseInt(d.label, 10);
                if (labelNum >= 8 && labelNum <= 11) {
                    overlayText = `- Sign  +ve Exponent`; // Negative sign, +ve exponent
                } else if (labelNum >= 12 && labelNum <= 15) {
                    overlayText = `- Sign  -ve Exponent`; // Negative sign, -ve exponent
                } else if (labelNum >= 0 && labelNum <= 4) {
                    overlayText = `+ Sign  +ve Exponent`; // +ve sign, +ve exponent
                } else if (labelNum >= 4 && labelNum <= 7) {
                    overlayText = `+ Sign  -ve Exponent`; // +ve sign, -ve exponent
                }

                // Set tooltip position and content
                const tooltip = d3.select("#tooltip");
                tooltip.style("visibility", "visible")
                    .style("left", (event.pageX + 5) + "px")
                    .style("top", (event.pageY - 5) + "px")
                    .text(overlayText);
            }
        })
        .on("mousemove", function (event) {
            const tooltip = d3.select("#tooltip");
            tooltip.style("left", (event.pageX + 5) + "px")
                .style("top", (event.pageY - 5) + "px");
        })
        .on("mouseout", function (event, d) {
            // Revert color
            d3.select(this).attr("fill", color_picker[parent_color_idx]);
            // Hide tooltip
            d3.select("#tooltip").style("visibility", "hidden");
        })
        .on("click", function (event, d) {
            if (container_id == "Level0-plot-container") {
                document.getElementsByClassName('show-arrow2')[0].style.display = 'block';
                document.querySelector(".level-3-button").style.display = "flex";

                plotLevel1part1(d.label, d.originalCount, scaleType, msb_4_bits); // Use original count on click
            } else if (container_id == "Level1-plot-container") {
                document.getElementsByClassName('show-arrow3')[0].style.display = 'block';
                document.querySelector(".level-4-button").style.display = "flex";

                plotLevel1part2(d.label, d.originalCount, scaleType, msb_4_bits); // Use original count on clicks
            }
        });

    // Display the original count on top of the bars
    svg.selectAll(".count-label")
        .data(plotData.filter(d => d.count !== null)) // Filter for non-zero originalCount
        .enter()
        .append("text")
        .attr("class", "count-label")
        .attr("x", d => x(d.label) + x.bandwidth() / 2)
        .attr("y", d => {
            const barHeight = height - y(d.count);
            const yPos = y(d.count) - 5; // Default y position above the bar
            return barHeight < minHeight ? y(d.count) - (minHeight - barHeight) - 5 : yPos;
        })
        .attr("text-anchor", "middle")
        .text(d => d.originalCount > 0 ? d.originalCount.toLocaleString() : '') // Format count with commas
        .on("click", function (event, d) {
            if (container_id == "Level0-plot-container") {
                plotLevel1part1(d.label, d.originalCount, scaleType, msb_4_bits); // Use original count on click
            } else if (container_id == "Level1-plot-container") {
                plotLevel1part2(d.label, d.originalCount, scaleType, msb_4_bits); // Use original count on clicks
            }
        })
        .on("mouseover", function (e, d) {
            d3.select(this).style("cursor", "pointer");
        })
        .on("mouseout", function (e, d) {
            d3.select(this).style("cursor", "default");
        });
}


// Example click handler function
function plotLevel0part2(label, count, matrixData, scaleType) {
    let temp_pos = positiveExponentSortedData.filter(item => item["first_half_4msb_index"] === label)
    let temp_neg = negativeExponentSortedData.filter(item => item["first_half_4msb_index"] === label)
    posMapIndexToFirstHalf4LSB = null
    negMapIndexToFirstHalf4LSB = null

    if (temp_pos.length !== 0) {
        posMapIndexToFirstHalf4LSB = getMinMaxApproxValues(temp_pos, "first_half_4lsb_index")
    } else if (temp_neg.length !== 0) {
        negMapIndexToFirstHalf4LSB = getMinMaxApproxValues(temp_neg, "first_half_4lsb_index")
    }
    document.querySelector(".level-2-button").style.display = "flex";
    drawPlot16("Level0-plot-container", matrixData[label], scaleType, label);
}

const intToBinary = (number) => {
    return (number >>> 0).toString(2).padStart(4, '0');
}

const plotLevel1part1 = async (lsb_4_bits, count, scaleType, msb_4_bits) => {
    let msb_binary = intToBinary(msb_4_bits);
    let lsb_binary = intToBinary(lsb_4_bits);

    const bin_8_bits = msb_binary + lsb_binary;
    const l0_bucket = parseInt(bin_8_bits, 2);

    const myHeaders = new Headers();
    myHeaders.append("Content-Type", "application/json");

    const raw = JSON.stringify({
        "top_8": l0_bucket
    });

    const requestOptions = {
        method: "POST",
        headers: myHeaders,
        body: raw,
        redirect: "follow"
    };

    l1_data = await makeApiRequest(`/drill/`, requestOptions)
    let temp_pos = positiveExponentSortedData.filter(item => item["first_half_index"] == l0_bucket)
    let temp_neg = negativeExponentSortedData.filter(item => item["first_half_index"] == l0_bucket)
    posMapIndexToSecondHalf4MSB = null
    negMapIndexToSecondHalf4MSB = null

    if (temp_pos.length !== 0) {
        posMapIndexToSecondHalf4MSB = getMinMaxApproxValues(temp_pos, "second_half_4msb_index")
    } else if (temp_neg.length !== 0) {
        negMapIndexToSecondHalf4MSB = getMinMaxApproxValues(temp_neg, "second_half_4msb_index")
    }
    drawPlot16("Level1-plot-container", l1_data[0], scaleType, l0_bucket);
};

const plotLevel1part2 = (label, count, scaleType, msb_4_bits) => {
    let temp_pos = positiveExponentSortedData.filter(item => item["first_half_index"] == msb_4_bits && item["second_half_4msb_index"] == label)
    let temp_neg = negativeExponentSortedData.filter(item => item["first_half_index"] == msb_4_bits && item["second_half_4msb_index"] == label)
    posMapIndexToSecondHalf4LSB = null
    negMapIndexToSecondHalf4LSB = null

    if (temp_pos.length !== 0) {
        posMapIndexToSecondHalf4LSB = getMinMaxApproxValues(temp_pos, "second_half_4lsb_index")
    } else if (temp_neg.length !== 0) {
        negMapIndexToSecondHalf4LSB = getMinMaxApproxValues(temp_neg, "second_half_4lsb_index")
    }
    drawPlot16("Level1-plot-container2", l1_data[1][label], scaleType, msb_4_bits)
}

const getMinMaxApproxValues = (sortedData, group_key) => {
    let groupedData = [];
    // Initialize with the first item's values
    let currentId = sortedData[0][group_key];
    let minApprox = sortedData[0].approx_value;
    let maxApprox = sortedData[0].approx_value;
    // Loop through the sorted data
    sortedData.forEach((item, index) => {
        if (item[group_key] !== currentId) {
            // Push the completed group's min and max to groupedData
            groupedData.push({ label: currentId, min: formatNumber(minApprox), max: formatNumber(maxApprox) });
            // Start a new group with the current item
            currentId = item[group_key];
            minApprox = item.approx_value;
            maxApprox = item.approx_value;
        } else {
            // Update max for the current group
            maxApprox = item.approx_value;
        }
        // If we're at the last item, push the final group's data
        if (index === sortedData.length - 1) {
            groupedData.push({ label: currentId, min: formatNumber(minApprox), max: formatNumber(maxApprox) });
        }
    });
    return groupedData;
}

function consolidateGroupedData(groupedData) {
    const consolidatedData = {};

    // Loop through each entry in groupedData
    groupedData.forEach(entry => {
        const id = entry.label;
        
        // If the id already exists in consolidatedData, update min and max
        if (consolidatedData[id]) {
            consolidatedData[id].min = Math.min(consolidatedData[id].min, entry.min);
            consolidatedData[id].max = Math.max(consolidatedData[id].max, entry.max);
        } else {
            // Otherwise, add the new id with its min and max
            consolidatedData[id] = { min: entry.min, max: entry.max };
        }
    });

    // Convert the consolidatedData object back to an array
    return Object.keys(consolidatedData).map(id => ({
        label: id,
        min: consolidatedData[id].min,
        max: consolidatedData[id].max
    }));
}

const loadExponentDataGlobally = async () => {
    d3.json("../static/json/positive_exponent_combinations_1DxF.json").then(positiveExponentData => {
        // Sort by 'approx_value' key
        positiveExponentSortedData = positiveExponentData.sort((a, b) => a.approx_value - b.approx_value);
        // {label: 3, min: "", max: ""},
        posMapIndexToFirstHalf4MSB = getMinMaxApproxValues(positiveExponentSortedData, "first_half_4msb_index");

    }).catch(error => {
        console.error("Error loading the JSON file:", error);
    });

    d3.json("../static/json/negative_exponent_combinations_1DxF.json").then(negativeExponentData => {
        // Sort by 'approx_value' key
        negativeExponentSortedData = negativeExponentData.sort((a, b) => a.approx_value - b.approx_value);
        // {label: 3, min: "", max: ""},
        negMapIndexToFirstHalf4MSB = getMinMaxApproxValues(negativeExponentSortedData, "first_half_4msb_index")
        negMapIndexToFirstHalf4MSB = consolidateGroupedData(negMapIndexToFirstHalf4MSB)
        negMapIndexToFirstHalf4MSB = negMapIndexToFirstHalf4MSB.sort((a, b) => a.min - b.min);

    }).catch(error => {
        console.error("Error loading the JSON file:", error);
    });
}

const formatNumber = (value) => {
    // Check if the value is greater than 5 digits
    if (Math.abs(value) < 100000) {
        // Round the value to 5 significant digits without forcing trailing zeros
        return Number.parseFloat(value).toPrecision(5);
    } else {
        return Number.parseFloat(value).toExponential(4);  // Show in scientific notation
    }
}

function showLegend() {
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
        const data = isPositive ? posMapIndexToFirstHalf4MSB : negMapIndexToFirstHalf4MSB;

        // Create table rows for the range values
        const tbody = table.append('tbody');
        data.forEach(item => {
            const row = tbody.append('tr');
            row.append('td').text(labelToChar[item.label]).style('border', '1px solid #ddd').style('padding', '8px');
            row.append('td').text(`${item.min} to ${item.max}`).style('border', '1px solid #ddd').style('padding', '8px');
        });
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
    displayExponentData();

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
    function displayExponentData() {
        // Clear the table body
        table.selectAll('tbody').remove();

        // Get the correct dataset (positive or negative)
        const data = posMapIndexToFirstHalf4LSB !== null ? posMapIndexToFirstHalf4LSB : negMapIndexToFirstHalf4LSB;
        // Create table rows for the range values
        const tbody = table.append('tbody');
        data.forEach(item => {
            const row = tbody.append('tr');
            row.append('td').text(item.label).style('border', '1px solid #ddd').style('padding', '8px');
            row.append('td').text(`${item.min} to ${item.max}`).style('border', '1px solid #ddd').style('padding', '8px');
        });
    }

}

function showLegend3() {
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
    displayExponentData();

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
    function displayExponentData() {
        // Clear the table body
        table.selectAll('tbody').remove();

        // Get the correct dataset (positive or negative)
        const data = posMapIndexToSecondHalf4MSB !== null ? posMapIndexToSecondHalf4MSB : negMapIndexToSecondHalf4MSB;
        // Create table rows for the range values
        const tbody = table.append('tbody');
        data.forEach(item => {
            const row = tbody.append('tr');
            row.append('td').text(item.label).style('border', '1px solid #ddd').style('padding', '8px');
            row.append('td').text(`${item.min} to ${item.max}`).style('border', '1px solid #ddd').style('padding', '8px');
        });
    }

}

function showLegend4() {
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
    displayExponentData();

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
    function displayExponentData() {
        // Clear the table body
        table.selectAll('tbody').remove();

        // Get the correct dataset (positive or negative)
        const data = posMapIndexToSecondHalf4LSB !== null ? posMapIndexToSecondHalf4LSB : negMapIndexToSecondHalf4LSB;
        // Create table rows for the range values
        const tbody = table.append('tbody');
        data.forEach(item => {
            const row = tbody.append('tr');
            row.append('td').text(item.label).style('border', '1px solid #ddd').style('padding', '8px');
            row.append('td').text(`${item.min} to ${item.max}`).style('border', '1px solid #ddd').style('padding', '8px');
        });
    }

}

document.addEventListener('DOMContentLoaded', async () => {
    await loadPlots('linear');
    await loadExponentDataGlobally();
});