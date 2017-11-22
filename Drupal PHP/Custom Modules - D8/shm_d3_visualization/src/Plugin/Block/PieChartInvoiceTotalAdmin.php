<?php
namespace Drupal\shm_d3_visualization\Plugin\Block;
use Drupal\Core\Block\BlockBase;
use Drupal\Core\Form\FormBuilderInterface;
use Drupal\Core\Form\FormStateInterface;
/**
 * Provides block HTML for D3 to draw Pie Chart to display Jobs
 * on Admin Dashboard.
 *
 * @Block(
 *   id = "pie_chart_invoice_total_admin",
 *   admin_label = @Translation("Pie Chart Invoice Total Admin"),
 *   category = "Dashboard Visualization"
 * )
 */

class PieChartInvoiceTotalAdmin extends  BlockBase{
  public function build() {
    return array(
        /*'#type' => 'inline_template',
        '#template' => '<div class = "admin dashboard jobs pie-chart">
                    <svg id = "pie-chart-jobs-admin"></svg></div>',
        '#context' => array(),
        */
        '#markup' => '<div class="panel panel-info">
						<div class="panel-heading">
							<h3 class="panel-title">Invoice Total</h3>
						</div>
						<div class="panel-body">
							<div id = "pie-chart-invoice-total-admin" style = "position:relative;">
							<div id="tooltip-admin-invoice-total-pie-chart" class="chart tool-tip hidden">
							<p><strong>Important Label Heading</strong></p>
							<p><span id="value">100</span></p>
						</div>
						</div>
					</div>',
        '#attached' => array(
            'library' => array(
                'shm_d3_visualization/pie_chart_invoice_total_admin',
                'shm_d3_visualization/shm_d3_visualization'
            )
        )
    );
  }

  /**
   * {@inheritdoc}
   */
  public function blockForm($form, FormStateInterface $form_state) {
    $form = parent::blockForm($form, $form_state);
    return $form;
  }
}