/**
 * Created by Sandeep on 25-07-2017.
 */

var app = angular.module('liferaftJob',['shmAjs', 'autoCompleteModule', 'ngDialog']);
app.config(function($interpolateProvider,$locationProvider){
    $interpolateProvider.startSymbol('[[').endSymbol(']]');
    $locationProvider.html5Mode(true).hashPrefix('!');
});

app.filter('byDeleteFlag', function () {
    return function (gasCylinders, filter) {
        var validCylinders = [];
        if(gasCylinders) {
            for (i = 0; i < gasCylinders.length; i++) {
                if (gasCylinders[i].delete_flag != filter) {
                    validCylinders.push(gasCylinders[i]);
                }
            }
            return validCylinders;
        }
    }
});

app.controller('liferaftForm', ['$scope', 'liferaftJob', 'liferaft', 'ngDialog','$location','editLiferaft','$window','serviceengineer', function($scope, liferaftJob, liferaft, ngDialog,$location,editLiferaft,$window,serviceengineer){
    $scope.job = {};
    $scope.result = {};
    $scope.job.liferaft = {};
    $scope.job.invoice = {};
    $scope.lifed_eq = {};
    $scope.showSuccess = false;
    $scope.showError = false;
    $scope.job.moderation_state = '';
    $scope.job.gas_cylinders=[{cylinder_sn:"",
        lpt:"",
        ptest_req:"",
        refill_req:"",
        full_weight:"",
        tare_weight:"",
        gas_weight_n2:"",
        gas_weight_co2:"",
        actual_full_weight:"",
        delete_flag:"",
    }];
    $scope.job.lifed_equipments = [{item:"",
        qty:"",
        manuf:"",
        lot_no:"",
        expiry:"",
        replaced_qty:"",
        replaced_manuf:"",
        replaced_lot_no:"",
        replaced_expiry:"",
        delete_flag:"",
    }];
    $scope.liferaftAutocompleteSuggestions = null;
    $scope.job.field_service_engineer ={};
    $scope.addGasCylinder = function(){
        $scope.job.gas_cylinders.push({cylinder_sn:""});
    };
    $scope.job.gas_cylinders.delete_flag = false;
    $scope.removeGasCylinder = function(gas_cylinder){
        for (var i=0;i<$scope.job.gas_cylinders.length;i++){
            if($scope.job.gas_cylinders[i]===gas_cylinder){
                //$scope.job.gas_cylinders.splice(i,1);
                $scope.job.gas_cylinders[i].delete_flag = true;
            }
        }
    };


    $scope.job.lifed_equipments = [{"item":"Water"},
        {"item":"Ration"},
        {"item":"Hand Flare"},
        {"item":"Parachute Rockets"},
        {"item":"Smoke Floats"},
        {"item":"First Aid Kit"},
        {"item":"ASS Tablets"},
        {"item":"Lithium Cell"}];

    $scope.removeLifedEquip=function (lifed_equip) {
        for (var i=0;i<$scope.job.lifed_equipments.length;i++){
            if($scope.job.lifed_equipments[i]===lifed_equip){
                //$scope.job.lifed_equipments.splice(i,1);
                $scope.job.lifed_equipments[i].delete_flag = true;
            }
        }
    };
    $scope.addLifedEquip=function(){
        $scope.job.lifed_equipments.push({"item":""});
    };

    $scope.onLRSelect = function(selectedLiferaft){
        var lrDetails = liferaft.getDetails(selectedLiferaft.nid);
        $scope.job.liferaft = lrDetails;
    };

    $scope.getMatchedLiferaft ={
        minimumChars: 2,
        data: function (searchText) {
            return liferaft.getMatched(searchText, 'liferaft');
        },
        itemTemplate: "<table class='auto-complete'> " +
        "<tbody>" +
        "<tr>" +
        "<td style='width: 30%;'>[[item.title | uppercase]]</td>" +
        "</tr>" +
        "</tbody>" +
        "</table>",
        containerCssClass: 'liferaft-autocomplete',
        selectedTextAttr: 'title',
        itemSelected: function(item){

            var dataPromise = liferaft.getDetails(item.item.nid);
            dataPromise.then(function(result) {
                $scope.job = result;
                $scope.job.liferaft.isNew = false;
            });
        }
    };



    $scope.getMatchedVessel ={
        minimumChars: 2,
        data: function (searchText) {
            return liferaft.getMatched(searchText,'lr-vessel');
        },
        itemTemplate: "<div class='auto-complete ui-autocomplete'> " +

        "<span style='width: 30%;'>[[item.title | uppercase]]</span>" +

        "</div>",
        containerCssClass: 'vessel-autocomplete',
        selectedTextAttr: 'title',
        itemSelected: function(item){
            $scope.newLiferaft.field_liferaft_name_vessel = item;
        },
    };

    $scope.getMatchedLiferaftManufacturer ={
        minimumChars: 2,
        data: function (searchText) {
            return liferaft.getMatched(searchText,'lr-manufacturer');
        },
        itemTemplate: "<div class='auto-complete ui-autocomplete'> " +

        "<span class='ui-menu-item' style='width: 30%;'>[[item.title | uppercase]]</span>" +

        "</div>",
        containerCssClass: 'liferaft-manufacturer-autocomplete',
        selectedTextAttr: 'title',
        itemSelected: function(item){
            newLiferaft.field_liferaft_manufacturer = item;
        }
    };

    $scope.addNewLiferaft = function () {
        $scope;
        var dia = ngDialog.open(
            {
                template: 'new-liferaft-dialog',
                className: 'ngdialog-theme-default',
                controller: 'dialogCtrl',
                scope: $scope,
                closeByDocument: false,
                /*preCloseCallback: function(value) {
                    if (confirm('Are you sure you want to close without saving your changes?')) {
                        return true;
                    }
                    return false;
                }*/
            }
        );
    };

    var path = $location.path();

    if(path.match(/^\/liferaft\/\d+\/edit$/))
    {
        var values = path.split("/");
        var entity_id = values[2];
        var promise = editLiferaft.getDetails(entity_id);
        promise.then(function (result) {
            Object.assign($scope.job, result);
            var a=10;
        });

    }

    var path_new_job = $location.url();
    if(path_new_job.match(/^\/liferaft\/new-job\?lr_id\=\d+$/))
    {
        var query_str =$location.search();
        var lr_id_query = query_str.lr_id;
        var dataPromise = liferaft.getDetails(lr_id_query);
        dataPromise.then(function(result) {
            if(result.status == 200){
                $scope.job = result;
            }

            var a = 10;

        });
    }

    $scope.saveJob = function(){

        var path = $location.path();

            if(path.match(/^\/liferaft\/\d+\/edit$/)) {
                $scope.job.modeartion_state = 'approved';
                if($scope.job.liferaft.field_liferaft_serial_no) {
                    alert('save clicked');
                    var values = path.split("/");
                    var entity_id = values[2];
                    var promise = editLiferaft.updateLiferaftJob($scope.job, entity_id);
                    promise.then(function (result) {
                        $window.location.href = '/service/' + entity_id;
                    });
                }
                else
                {
                    $scope.job.showError = true;
                }
        }
        else {
            console.log(JSON.stringify($scope.job));
                if($scope.job.liferaft.field_liferaft_serial_no) {
            var promise = liferaftJob.createJob($scope.job);
            promise.then(function (result) {
                Object.assign($scope.result, result);
                console.log($scope.result);
                var entity_id = $scope.result.data;
                $window.location.href ='/service/'+entity_id;
            });
            $scope.job.showSuccess = true;
                }
                else
                {
                    $scope.job.showError = true;
                }
        }
    };

    $scope.draftJob = function () {

        var path = $location.path();

        if(path.match(/^\/liferaft\/\d+\/edit$/)) {
            if($scope.job.liferaft.field_liferaft_serial_no) {
                $scope.job.modeartion_state = 'draft';
                alert('save clicked');
                var values = path.split("/");
                var entity_id = values[2];
                var promise = editLiferaft.updateLiferaftJob($scope.job, entity_id);
                promise.then(function (result) {
                    $window.location.href = '/service/' + entity_id;
                });
            }
            else
            {
                $scope.job.showError = true;
            }
        }
        else {


                if($scope.job.liferaft.field_liferaft_serial_no) {
            console.log(JSON.stringify($scope.job));
            $scope.job.modeartion_state = 'draft';
            var promise = liferaftJob.createJob($scope.job);
            promise.then(function (result) {
                Object.assign($scope.result, result);
                console.log($scope.result);
                var entity_id = $scope.result.data;
                $window.location.href ='/service/'+entity_id;
            });
                $scope.job.showSuccess = true;
            }
            else
            {
                $scope.job.showError = true;
            }


        }

    }
    $scope.sendForAprrovalJob = function () {

        var path = $location.path();

        if (path.match(/^\/liferaft\/\d+\/edit$/)) {
            if($scope.job.liferaft.field_liferaft_serial_no) {
                $scope.job.modeartion_state = 'pending_approval';
                alert('save clicked');
                var values = path.split("/");
                var entity_id = values[2];
                var promise = editLiferaft.updateLiferaftJob($scope.job, entity_id);
                promise.then(function (result) {
                    $window.location.href = '/service/' + entity_id;
                });
            }
            else
                {
                    $scope.job.showError = true;
                }
        }
        else {
            console.log(JSON.stringify($scope.job));
            if($scope.job.liferaft.field_liferaft_serial_no) {
            $scope.job.modeartion_state = 'pending_approval';
            var promise = liferaftJob.createJob($scope.job);
            promise.then(function (result) {
                Object.assign($scope.result, result);
                console.log($scope.result);
                var entity_id = $scope.result.data;
                $window.location.href = '/service/' + entity_id;
            });
            $scope.job.showSuccess = true;
            }
            else
            {
                $scope.job.showError = true;
            }
        }
    }
    $scope.approveJob = function () {
        var path = $location.path();

        if(path.match(/^\/liferaft\/\d+\/edit$/)) {
            $scope.job.modeartion_state = 'approved';
            alert('save clicked');
            var values = path.split("/");
            var entity_id = values[2];
            var promise =  editLiferaft.updateLiferaftJob($scope.job,entity_id);
            promise.then(function (result) {
                $window.location.href ='/service/'+entity_id;
            });
        }
    }
    $scope.rejectedJob = function () {
        var path = $location.path();

        if(path.match(/^\/liferaft\/\d+\/edit$/)) {
            $scope.job.modeartion_state = 'draft';
            alert('save clicked');
            var values = path.split("/");
            var entity_id = values[2];
            var promise =  editLiferaft.updateLiferaftJob($scope.job,entity_id);
            promise.then(function (result) {
                $window.location.href ='/service/'+entity_id;
            });
        }
    }
    $scope.getMatchedServiceEngineer ={
        minimumChars: 2,
        data: function (searchText) {
            return liferaft.getMatched(searchText, 'service-engineer');
        },
        itemTemplate: "<div class='auto-complete ui-autocomplete'> " +

        "<span class='ui-menu-item' style='width: 30%;'>[[item.name | uppercase]]</span>" +

        "</div>",
        containerCssClass: 'liferaft-autocomplete',
        selectedTextAttr: 'name',
        itemSelected: function(item) {
            $scope.job.field_service_engineer = item.item;

        },
    };


}]);
app.controller('dialogCtrl', ['$scope', 'ngDialog','autocompleteProvider', 'vessel', function($scope, ngDialog, autocompleteProvider, vessel){
    //$scope = $scope.$parent;
    $scope.newLiferaft = {};
    $scope.newLiferaft.vessel={};
    $scope.saveNewLiferaft = function(){
        $scope.$parent.job.liferaft = $scope.newLiferaft;
        $scope.$parent.job.liferaft.isNew = true;
        ngDialog.close();
    };

    $scope.getVesselDetails = function (id) {
        var promise = vessel.getDetails(id);
        promise.then(function (result) {
           return result;
        });
    }



    $scope.getMatchedVessel ={
        minimumChars: 2,
        data: function (searchText) {
            return autocompleteProvider.getMatched(searchText,'lr-vessel');
        },
        itemTemplate: "<div class='auto-complete ui-autocomplete'> " +

        "<span style='width: 30%;'>[[item.title | uppercase]]</span>" +

        "</div>",
        containerCssClass: 'vessel-autocomplete',
        selectedTextAttr: 'title',
        itemSelected: function(item){
            $scope.newLiferaft.vessel = item.item;
            var promise = vessel.getDetails($scope.newLiferaft.vessel.nid);
            promise.then(function (result) {
                Object.assign($scope.newLiferaft.vessel, result);
            });
        },
    };

    $scope.getMatchedLiferaftManufacturer ={
        minimumChars: 2,
        data: function (searchText) {
            return autocompleteProvider.getMatched(searchText,'lr-manufacturer');
        },
        itemTemplate: "<div class='auto-complete ui-autocomplete'> " +

        "<span class='ui-menu-item' style='width: 30%;'>[[item.title | uppercase]]</span>" +

        "</div>",
        containerCssClass: 'liferaft-manufacturer-autocomplete',
        selectedTextAttr: 'title',
        itemSelected: function(item){
            $scope.newLiferaft.field_liferaft_manufacturer = item.item;
        }
    };

    $scope.getMatchedLiferaftTypeAndCapacity = {
        minimumChars: 2,
        data: function (searchText) {
            var type = 'liferaft-type';
            return autocompleteProvider.getMatched(searchText,type);
        },
        itemTemplate: "<div class='auto-complete ui-autocomplete'> " +

        "<span class='ui-menu-item' style='width: 30%;'>[[item.type_capacity | uppercase]]</span>" +

        "</div>",
        containerCssClass: 'liferaft-type-autocomplete',
        selectedTextAttr: 'type_capacity',
        itemSelected: function (item) {
            $scope.newLiferaft.field_liferaft_type = item.item;
        }
    };
}]);