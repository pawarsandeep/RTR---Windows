/**
 * @file Draws Invoice Pie Chart on Admin Dashboard.
 * @author Sandeep Pawar
 */
(function() {
var width = 230;
var height = 250;
var marginx = 80;
var marginy = 30;
var radius = (height - marginx)/2;
var color = d3.scaleOrdinal(d3.schemeCategory20b);
var svg = d3.select('#pie-chart-invoice-total-admin')
	.append('svg')
	.attr('width', width+20)
	.attr('height', height+10)
	.append('g')
	.attr('transform', 'translate(' + (width-70) + ',' + (height/2) + ')');
var legend = d3.select("#pie-chart-invoice-total-admin svg")
	.append("g")
	.attr("transform", "translate(95,20)")
	.attr("class", "legend-container");
var arc = d3.arc()
	.innerRadius(0)
	.outerRadius(radius);
var pie = d3.pie()
	.value(function(d) {return d.count;})
	.sort(null);
d3.json('/rest-export/admin-invoice-total', function(error, data) {
	console.log(data);
	var g = svg.selectAll('path')
	.data(pie(data))
	.enter()
	.append('g')
	.attr('class', 'arc');
	var p = g.append('path')
		.attr('d', arc)
		.attr('fill', function(d, i) {
			return color(color(i));
		});
	g.append('text')
		.attr('transform', function(d) {
			return "translate(" + arc.centroid(d) + ")";
		})
		.attr("dy", ".35em")
		.text(function(d, i) {
			return data[i].count;
		})
		.attr("fill","white");
		
	var leg = legend.selectAll(".legend")
	.data(pie(data))
	.enter()
	.append("g")
	.attr("class", "legend")
	.attr("transform", function(d,i) {
		return 'translate(' + 80*(i) + ',' + 220 + ')';
	});
	leg.append("rect")
	.attr("height", 10)
	.attr("width", 10)
	.attr("fill", function(d,i) {
		return color(color(i));
	});
	leg.append("text")
	.text(function(d, i){
		return data[i].status;
	})
	.attr("transform", "translate(15,10)");
	g.on("mouseenter", function(d) {
		d3.select("#tooltip-admin-invoice-total-pie-chart")
		.style("left", d3.event.pageX-100 + "px")
        .style("top", d3.event.pageY-200 + "px")
        .classed("hidden", false)
        .style("position", "absolute")
        .select('strong')
        .text(d.data.status)
        d3.select("#tooltip-admin-invoice-total-pie-chart")
        .select('#value')
        .text(d.data.count);
	});
	g.on("mouseleave", function(d) {
		d3.select("#tooltip-admin-invoice-total-pie-chart")
		.classed("hidden", true);
	});
	p.on("mouseenter", function(d){
			var c = arc.centroid(d),
			x = c[0],
			y = c[1],
			h = Math.sqrt(x*x + y*y),
			pullOutSize = parseInt(5, 10);
			d3.select(this)
			.transition()
			.duration(500)
			.attr("transform", "translate(" + ((x/h) * pullOutSize) + "," + 
					((y/h) * pullOutSize) + ")")
			.attr("stroke", "white")
			.attr("stroke-width", 4);
			

		});
	p.on("mouseleave", function(d) {
		d3.select(this)
		.transition()
		.duration(500)
		.attr("transform", "translate(0,0)")
		.attr("stroke-width", 0);

	});
	});
	
}());